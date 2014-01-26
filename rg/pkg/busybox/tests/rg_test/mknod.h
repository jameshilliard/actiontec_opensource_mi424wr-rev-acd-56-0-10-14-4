#ifdef TEST_SOURCE
#include "test_tool.h"
#include <linux/kdev_t.h>

int fn_mknod(void)
{
#ifndef BB_MKNOD
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;
    struct stat st;

    activate("mknod -m a+r nb b 3 2", NULL);
    if (stat("nb", &st))
	goto Exit;
    if (MAJOR(st.st_rdev) != 3 || MINOR(st.st_rdev) != 2)
	goto Exit;
    if (!(S_IFBLK & st.st_mode))
	goto Exit;

    activate("mknod -m r np p", NULL);
    if (stat("np", &st))
	goto Exit;
    if (!(S_IFIFO & st.st_mode))
	goto Exit;

    activate("mknod -m w nc c 4 5", NULL);
    if (stat("nc", &st))
	goto Exit;
    if (MAJOR(st.st_rdev) != 4 || MINOR(st.st_rdev) != 5)
	goto Exit;
    if (!(S_IFCHR & st.st_mode))
	goto Exit;

    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}
#endif

#ifdef SET_ENTRIES
register_test("mknod", fn_mknod, 1);
#endif
