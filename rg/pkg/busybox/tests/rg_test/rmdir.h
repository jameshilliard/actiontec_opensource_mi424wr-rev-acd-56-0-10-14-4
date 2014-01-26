#ifdef TEST_SOURCE
#include "test_tool.h"
#include <sys/types.h>
#include <dirent.h>

const char rmdir_err_msg[] = "rmdir: a: ";

int fn_rmdir(void)
{
#ifndef BB_RMDIR
    return TEST_RES_NOT_INCLUDED;
#else
    char *e;
    int ret = TEST_RES_FAIL;
    int len1 = strlen(rmdir_err_msg);
    char *ne = strerror(ENOTEMPTY);
    int len2 = strlen(ne);
    int l = len1+len2+1;
    int el = l;

    create_path("a/b");
    
    activate_e("rmdir a", NULL, &e, &l, NULL, NULL);
    if (l!=el || memcmp(e, rmdir_err_msg, len1)	|| memcmp(e+len1, ne, len2) || 
	*(e+l-1) != '\n')
    {
	goto Exit;
    }

    if (!check_dir("a"))
	goto Exit;

    activate("rmdir a/b", NULL);
    if(check_dir("a/b"))
	goto Exit;

    activate("rmdir a", NULL);
    if (check_dir("a"))
	goto Exit;
	    
    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("rmdir", fn_rmdir, 1);
#endif
