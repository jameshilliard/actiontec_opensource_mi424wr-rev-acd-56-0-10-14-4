#ifdef TEST_SOURCE
#include "test_tool.h"

#define CHK_DIR(D) \
    if (!check_dir(D)) \
	goto Exit;

int fn_mkdir(void)
{
#ifndef BB_MKDIR
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;
    
    activate("mkdir mkdir1_a", NULL);
    CHK_DIR("mkdir1_a");
    
    activate("mkdir mkdir1_a/b", NULL);
    CHK_DIR("mkdir1_a/b");
    
    activate("mkdir -p mkdir1_b/a/b/c", NULL);
    CHK_DIR("mkdir1_b/a/b/c");
    
    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("mkdir", fn_mkdir, 1);
#endif
