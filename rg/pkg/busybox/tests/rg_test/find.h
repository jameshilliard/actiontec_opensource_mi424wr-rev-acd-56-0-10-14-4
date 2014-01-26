#ifdef TEST_SOURCE
#include "test_tool.h"

const char find_out0[] = "./a/b/c/f1\n";
const char find_out1[] = "./a/b/c/f1\n./a/la/c/f1\n";

int fn_find(void)
{
#ifndef BB_FIND
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;

    create_path("a/b/c");
    copy_file("../file1.txt", "a/b/c/f1");
    chdir("a");
    symlink("b", "la");
    chdir("..");
    RUN_COMPARE("find . -name f*", find_out0);
    RUN_COMPARE("find . -name *1*", find_out0);
    RUN_COMPARE("find . -follow -name f1", find_out1);
    
    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("find", fn_find, 1);
#endif
