#ifdef TEST_SOURCE
#include "test_tool.h"

const char sort_prp[] = 
{
#include "sort.prp.val"
    '\0'
};

const char sort_r_prp[] = 
{
#include "sort_r.prp.val"
    '\0'
};

int fn_sort(void)
{
#ifndef BB_SORT
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;

    RUN_COMPARE("sort file1.txt", sort_prp);
    RUN_COMPARE("sort -r file1.txt", sort_r_prp);

    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("sort", fn_sort, 0);
#endif
