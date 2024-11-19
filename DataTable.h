#pragma once

typedef struct DataTable DataTable;

DataTable * DataTable_create(void);
void DataTable_destroy(DataTable * d);
void DataTable_setDefaultElementSize(DataTable * d, int size);
void DataTable_resize(DataTable * d, int newbucketamount);

typedef enum EDATA_TABLE_OPERATION {
    EDTO_FIND,
    EDTO_FIND_OR_ADD,
    EDTO_REMOVE,
    // TODO: EDTO_REPLACE
} EDATA_TABLE_OPERATION;

void * DataTable_operation(DataTable * d, EDATA_TABLE_OPERATION op, const char * key, int keylen, int datasize, int * outdatasize);
