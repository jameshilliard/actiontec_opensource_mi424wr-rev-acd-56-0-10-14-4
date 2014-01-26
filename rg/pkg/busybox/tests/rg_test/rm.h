#ifdef TEST_SOURCE
#include "test_tool.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

int fn_rm(void)
{
#ifndef BB_RM
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_SUCCESS;
    struct stat st;
    
    copy_file("../file1.txt", "f1");
    create_path("a/b/c");
    copy_file("f1", "a/b/f1");

    activate("rm -r a", NULL);
    if (check_dir("a"))
	goto Exit;
    
    activate("rm f1", NULL);
    if (!stat("f1", &st))
	goto Exit;
    
    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("rm", fn_rm, 1);
#endif
