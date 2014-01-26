#ifdef TEST_SOURCE
#include "test_tool.h"

const char head_f1_prp[] = 
{
#include "head_f1.prp.val"
    '\0'
};

const char head_f1_f2_prp[] = 
{
#include "head_f1_f2.prp.val"
    '\0'
};

const char head_n3_prp[] = 
{
#include "head_n3.prp.val"
    '\0'
};

int fn_head(void)
{
#ifndef BB_HEAD
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;
  
    RUN_COMPARE("head file1.txt", head_f1_prp);
    RUN_COMPARE("head file1.txt file2.txt", head_f1_f2_prp);
    RUN_COMPARE("head -n 3 file1.txt", head_n3_prp);

    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}
#endif
#ifdef SET_ENTRIES
register_test("head", fn_head, 0);
#endif
