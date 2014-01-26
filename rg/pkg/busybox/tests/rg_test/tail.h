#ifdef TEST_SOURCE
#include "test_tool.h"

const char tail_f1_prp[] = 
{
#include "tail_f1.prp.val"
    '\0'
};

const char tail_f1_f2_prp[] = 
{
#include "tail_f1_f2.prp.val"
    '\0'
};

const char tail_n3_prp[] = 
{
#include "tail_n3.prp.val"
    '\0'
};

int fn_tail(void)
{
#ifndef BB_TAIL
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;

    RUN_COMPARE("tail file1.txt", tail_f1_prp);
    RUN_COMPARE("tail -n 3 file1.txt", tail_n3_prp);
    RUN_COMPARE("tail file1.txt file2.txt", tail_f1_f2_prp);

    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("tail", fn_tail, 0);
#endif
