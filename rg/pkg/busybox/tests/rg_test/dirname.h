#ifdef TEST_SOURCE
#include "test_tool.h"

int fn_dirname(void)
{
#ifndef BB_DIRNAME
    return TEST_RES_NOT_INCLUDED;
#else
    if (run_and_compare("dirname a/b/f", "a/b\n", 4))
	return TEST_RES_FAIL;
    if (run_and_compare("dirname dgadjhsd", ".\n", 2))
	return TEST_RES_FAIL;
    return TEST_RES_SUCCESS;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("dirname", fn_dirname, 0);
#endif
