#include "DataTable.h"
#include <stdio.h>

int main(int argc, char ** argv)
{
    DataTable * d = DataTable_create();

    for(int i = 1; i < argc; ++i)
    {
        int * x = (int*)DataTable_findOrAdd(d, argv[i]);
        *x = i;
    }

    for(int i = 1; i < argc; ++i)
    {
        const int * x = (const int*)DataTable_findOrAdd(d, argv[i]);
        printf("%d - %s\n", i, (*x == i) ? "ok" : "ERROR!!");
    }

    DataTable_destroy(d);
    return 0;
}
