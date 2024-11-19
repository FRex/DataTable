#pragma once

/* DataTable is opaque type the definition is in the .c file */
typedef struct DataTable DataTable;

/* create, destroy, resize and set default element size */
DataTable * DataTable_create(void);
void DataTable_destroy(DataTable * d);
void DataTable_resize(DataTable * d, int newbucketamount);
void DataTable_setDefaultElementSize(DataTable * d, int size);

/* high level API, shortcuts for DataTable_operation */
#define DataTable_find(d, key) DataTable_operation(d, EDTO_FIND, key, -1, -1, NULL)
#define DataTable_findOrAdd(d, key) DataTable_operation(d, EDTO_FIND_OR_ADD, key, -1, -1, NULL)
#define DataTable_remove(d, key) DataTable_operation(d, EDTO_REMOVE, key, -1, -1, NULL)

/* the low level API, all other apis are shortcuts using this one */
typedef enum EDATA_TABLE_OPERATION {
    EDTO_FIND,
    EDTO_FIND_OR_ADD,
    EDTO_REMOVE,
    /* TODO: EDTO_REPLACE? */
} EDATA_TABLE_OPERATION;

void * DataTable_operation(DataTable * d, EDATA_TABLE_OPERATION op, const char * key, int keylen, int datasize, int * outdatasize);
