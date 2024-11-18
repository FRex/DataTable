#pragma once

typedef struct DataTable DataTable;

DataTable * DataTable_create(void);
void DataTable_destroy(DataTable * d);
void DataTable_setDefaultElementSize(DataTable * d, int size);

// find the element by name, NULL if not existing
void * DataTable_find(const DataTable * d, const char * key);

// find element or add it if it doesn't exist, then return it
void * DataTable_findOrAdd(DataTable * d, const char * key);

void DataTable_rehash(DataTable * d, int newbucketamount);
