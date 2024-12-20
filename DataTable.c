#include "DataTable.h"
#include <stdlib.h> // for calloc, free, realloc
#include <string.h> // for strlen, memcmp, memcpy, etc.

struct DataTable {
    int arrlen;
    void ** arr;
    int elemdatasize;
};

DataTable * DataTable_create(void)
{
    DataTable * d = calloc(1, sizeof(DataTable));
    if(!d) return NULL;
    return d;
}

int DataTable_setDefaultElementSize(DataTable * d, int size)
{
    const int ret = d->elemdatasize;
    if(size > 0) d->elemdatasize = size;
    return ret;
}

struct DataTable_priv_ElementHeader {
    struct DataTable_priv_ElementHeader * next; // for separate chaining
    unsigned hash;
    int keylen;
    int datasize;

};

void DataTable_destroy(DataTable * d)
{
    for(int i = 0; i < d->arrlen; ++i)
    {
        struct DataTable_priv_ElementHeader * e = d->arr[i];
        if(!e) continue;
        struct DataTable_priv_ElementHeader * next = e->next;
        free(e);
        e = next;
    }

    free(d);
}

/* 32-bit fnv1a */
static unsigned DataTable_priv_fnv1a32(const char * str, int length)
{
    unsigned ret = 2166136261u;
    while(length > 0)
    {
        ret ^= (unsigned char)(*str);
        ret *= 16777619u;
        --length;
        ++str;
    } /* while length > 0 */

    return ret;
}

static int DataTable_priv_makeAlignedSize(int val) { return ((val + 7) / 8) * 8; }

void DataTable_resize(DataTable * d, int newbucketamount)
{
    void ** newarr = calloc(newbucketamount, sizeof(void*));
    if(!newarr) return;

    if(d->arrlen > 0) {
        for(int i = 0; i < d->arrlen; ++i)
        {
            struct DataTable_priv_ElementHeader * e = d->arr[i];
            while(e)
            {
                const size_t idx = (size_t)(e->hash % newbucketamount);
                struct DataTable_priv_ElementHeader * next = e->next;
                e->next = newarr[idx];
                newarr[idx] = e;
                e = next;
            }
        } // for i

        // TODO: also reverse all the chains in newarr since they were reversed in loop above?
    }

    d->arrlen = newbucketamount;
    d->arr = newarr;
}

static void * DataTable_priv_return(struct DataTable_priv_ElementHeader * e, int * outdatasize)
{
    if(e)
    {
        if(outdatasize) *outdatasize = e->datasize;
        return (char*)(e + 1) + DataTable_priv_makeAlignedSize(e->keylen);
    }

    if(outdatasize) *outdatasize = -1;
    return NULL;
}

void * DataTable_operation(DataTable * d, EDATA_TABLE_OPERATION op, const char * key, int keylen, int datasize, int * outdatasize)
{
    if(d->arrlen == 0)
    {
        switch(op)
        {
        case EDTO_FIND:
        case EDTO_REMOVE:
            // table is empty so cannot find or remove an element
            return DataTable_priv_return(NULL, outdatasize);

        case EDTO_FIND_OR_ADD:
            // table is empty but we need to add an element so get an array going
            DataTable_resize(d, 10);
            break;
        } // switch op
    } // if arrlen is 0

    // if key or data len is not positive then use defaults
    if(keylen <= 0) keylen = (int)strlen(key);
    if(datasize <= 0) datasize = d->elemdatasize;

    // hash and find index, once
    const unsigned hash = DataTable_priv_fnv1a32(key, keylen);
    const int idx = (int)(hash % d->arrlen);

    void ** eptr = &d->arr[idx];
    struct DataTable_priv_ElementHeader * e = d->arr[idx];
    while(e)
    {
        if(hash == e->hash && keylen == e->keylen && 0 == memcmp(key, (const char*)(e + 1), keylen))
            break; // e is the element we needed

        eptr = (void**)&e->next;
        e = e->next;
    }

    if(e)
    {
        void * ret = DataTable_priv_return(e, outdatasize);

        // no matter what we return the element data ptr and size, but in case of remove op also free it first
        switch(op)
        {
        case EDTO_REMOVE:
            *eptr = e->next;
            free(e);
            break;

        case EDTO_FIND_OR_ADD:
        case EDTO_FIND:
            break;
        } // switch op
        return ret;
    } // if e

    // by now we know we have e == NULL
    switch(op)
    {
    case EDTO_REMOVE:
    case EDTO_FIND:
        // return NULL and size -1
        return DataTable_priv_return(NULL, outdatasize);

    case EDTO_FIND_OR_ADD: // go on to allocate new element
        break;
    } // switch op

    // TODO: handle overflow here
    e = malloc(sizeof(struct DataTable_priv_ElementHeader) + DataTable_priv_makeAlignedSize(keylen) + datasize);
    if(!e)
        return DataTable_priv_return(NULL, outdatasize);

    e->hash = hash;
    e->keylen = keylen;
    e->datasize = datasize;
    // TODO: add 1 more char for nul sep for keys, even if not for values? or make that flag in creation of table, to add +1 nul ending to key/val?

    // copy the key in and initialize the data part to zero
    memcpy((e + 1), key, keylen);
    memset((char*)(e + 1) + DataTable_priv_makeAlignedSize(e->keylen), 0x0, datasize);

    // chain in the element
    e->next = d->arr[idx];
    d->arr[idx] = e;

    // TODO: check load factor per slot/bucket and rehash if needed?

    return DataTable_priv_return(e, outdatasize);
}
