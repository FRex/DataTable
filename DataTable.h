#pragma once

typedef struct DataTable DataTable;

// TODO: way to get extra storage before/after DataTable struct
// TODO: add ref counting or not?
DataTable * DataTable_create(void); // TODO: args like initial size, inline size, struct size (if not variable), etc.
void DataTable_destroy(DataTable * d); // TODO: make this a macro to zero out the pointer too?

// find the element by name, NULL if not existing
void * DataTable_find(const DataTable * d, const char * key);

// find element or add it if it doesn't exist, then return it
void * DataTable_findOrAdd(DataTable * d, const char * key);
// TODO: some 'remove and replace item' call too, via flags or new func?
// TODO: use macros to cast types and/or give struct size in _create or find?
// TODO: function versions with length given
// TODO: a delete method
// TODO: for memory error, callback err func with a possible longjmp/abort/throw in it, plus return a NULL if err func returns?

void DataTable_rehash(DataTable * d, int newbucketamount);
