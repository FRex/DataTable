#pragma once

typedef struct DataTable DataTable;

// TODO: way to get extra storage before/after DataTable struct
// TODO: add ref counting or not?
DataTable * DataTable_create(void);
void DataTable_destroy(DataTable * d);
