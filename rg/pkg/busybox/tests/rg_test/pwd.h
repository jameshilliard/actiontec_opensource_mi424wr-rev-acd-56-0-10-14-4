#ifdef TEST_SOURCE
#include "test_tool.h"

int fn_pwd(void)
{
#ifndef BB_PWD
    return TEST_RES_NOT_INCLUDED;
#else
    char wd[MAX_PATH_SIZE+1];
    int ret = TEST_RES_FAIL;
    char *p;
    int l;

    create_path("a");
    chdir("a");
    if (!getcwd(wd, MAX_PATH_SIZE))
	end_program(1, "cannot getcwd");
    strcat(wd, "\n"); 
    l = strlen(wd);
    p = activate("pwd", &l);
    if (l!=strlen(wd) || memcmp(wd, p, l))
	goto Exit;

    if (p[l-2]!='a')
	goto Exit;
    
    ret = TEST_RES_SUCCESS;
Exit:
    chdir("..");
    free(p);
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("pwd", fn_pwd, 1);
#endif
