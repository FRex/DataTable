#include "DataTable.h"
#include <stdio.h>

int main(int argc, char ** argv)
{
    DataTable * d = DataTable_create();
    DataTable_setDefaultElementSize(d, sizeof(int));

    for(int i = 1; i < argc; ++i)
    {
        int * x = (int*)DataTable_operation(d, EDTO_FIND_OR_ADD, argv[i], 0, 0, NULL);
        *x = i;
    }

    DataTable_resize(d, 2);

    for(int i = 1; i < argc; ++i)
    {
        const int * x = (const int*)DataTable_operation(d, EDTO_FIND, argv[i], 0, 0, NULL);
        printf("%d - %s\n", i, (*x == i) ? "ok" : "ERROR!!");
    }

    DataTable_destroy(d);
    return 0;
}
