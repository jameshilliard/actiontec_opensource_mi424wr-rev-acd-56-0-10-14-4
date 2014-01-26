#ifdef TEST_SOURCE
#include "test_tool.h"

int fn_cat(void)
{
#ifndef BB_CAT
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;
    int l = sizeof(file1_txt)+sizeof(file2_txt);
    char *p = activate("cat file1.txt file2.txt", &l);
    char *t = p + sizeof(file1_txt);

    if (l != sizeof(file1_txt)+sizeof(file2_txt))
	goto Exit;
    if (memcmp(p, file1_txt, sizeof(file1_txt)))
	goto Exit;
    if (memcmp(t, file2_txt, sizeof(file2_txt)))
	goto Exit;

    ret = TEST_RES_SUCCESS;
Exit:
    free(p);
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("cat", fn_cat, 0);
#endif
