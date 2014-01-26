#ifdef TEST_SOURCE
#include "test_tool.h"

#include <sys/stat.h>
#include <unistd.h>

int fn_touch(void)
{
#ifndef BB_TOUCH
    return TEST_RES_NOT_INCLUDED;
#else
    struct stat st;
    unsigned long mt;
    int ret = TEST_RES_FAIL;
    
    activate("touch empty", NULL);
    if (stat("empty", &st))
	goto Exit;
    mt = st.st_mtime;
    sleep(1);
    activate("touch empty", NULL);
    if (stat("empty", &st))
	goto Exit;
    /* could be 1 or 2 seconds difference */
    if (st.st_mtime!=mt+1 && st.st_mtime!=mt+2)
	goto Exit;
    
    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("touch", fn_touch, 1);
#endif
