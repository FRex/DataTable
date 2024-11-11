#include "DataTable.h"
#include <stdlib.h> // for calloc, free, realloc
#include <string.h> // for strlen, memcmp, memcpy, etc.

// TODO: add macros here to hook up other allocators
// TODO: compile with good warnings and as both c and c++ and run thru valgrind, ubsan, asan, etc.

struct DataTable {
    int arrlen; // TODO: make this later into a power of two thing to avoid a div/mod in find?
    void ** arr;
    // TODO: add hint in DataTable_create to have some inline arr storage here at first?
};

DataTable * DataTable_create(void)
{
    DataTable * d = calloc(1, sizeof(DataTable));
    if(!d) return NULL;

    // temp
    d->arrlen = 10;
    d->arr = calloc(d->arrlen, sizeof(void*));
    if(!d->arr)
    {
        free(d);
        return NULL;
    }
    // end temp

    return d;
}

void DataTable_destroy(DataTable * d)
{
    // TODO: implement
}

void * DataTable_find(const DataTable * d, const char * key)
{
    // TODO: implement
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

// TODO: for 64 bits try another layout? maybe few layouts depending on keylen value, could combine keylen with hash bits, etc.
struct DataTable_priv_ElementHeader {
    // TODO: try different layouts/order of elements here, e.g. in order of access like: hash, keylen, key, data, then
    struct DataTable_priv_ElementHeader * next; // for separate chaining
    unsigned hash;
    int keylen;
    size_t datalen;

};

static size_t DataTable_priv_makeAlignedSize(size_t val)
{
    return ((val + 7) / 8) * 8;
}

void * DataTable_findOrAdd(DataTable * d, const char * key)
{
    const int keylen = (int)strlen(key); // TODO: handle error if len is too big?
    const unsigned long long hash = DataTable_priv_fnv1a32(key, keylen);

    if(d->arrlen == 0) DataTable_rehash(d, 10);

    const size_t datalen = sizeof(int); // TODO: later this should be argument to here, so this class is a mapping name -> mem buffer of any size? or even mem -> mem? strings can contain embedded 0s

    const size_t idx = (size_t)(hash % d->arrlen);
    struct DataTable_priv_ElementHeader * e;
    for(e = d->arr[idx]; e; e = e->next)
        if(hash == e->hash && keylen == e->keylen && 0 == memcmp(key, (const char*)(e + 1), keylen))
            break; // e is the element we needed

    // we need crete and insert a new element, in front since its most recently added so make it most recently found when looking?
    if(!e)
    {
        // TODO: handle overflow here
        e = malloc(sizeof(struct DataTable_priv_ElementHeader) + DataTable_priv_makeAlignedSize(keylen) + datalen);
        if(!e) return NULL;

        e->hash = hash;
        e->keylen = keylen;
        e->datalen = datalen;
        // TODO: add 1 more char for nul sep for keys, even if not for values? or make that flag in creation of table, to add +1 nul ending to key/val?
        memcpy((e + 1), key, keylen);
        e->next = d->arr[idx];
        d->arr[idx] = e;
        memset((char*)(e + 1) + DataTable_priv_makeAlignedSize(e->keylen), 0x0, datalen);
    }

    // TODO: check load factor per slot/bucket and rehash if needed?

    return (char*)(e + 1) + DataTable_priv_makeAlignedSize(e->keylen);
}

void DataTable_rehash(DataTable * d, int newbucketamount)
{
    void ** newarr = calloc(newbucketamount, sizeof(void*));
    if(!newarr) return;

    // if we had nothing before, just assign new size and exit
    if(d->arrlen == 0)
    {
        d->arrlen = newbucketamount;
        return;
    }

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
    }

    // TODO: also reverse all the chains in newarr since they were reversed in loop above?
    d->arrlen = newbucketamount;
    d->arr = newarr;
}
