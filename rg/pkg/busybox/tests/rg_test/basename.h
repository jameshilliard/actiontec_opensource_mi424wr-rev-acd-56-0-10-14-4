#ifdef TEST_SOURCE
#include "test_tool.h"

int fn_basename(void)
{
#ifndef BB_BASENAME
    return TEST_RES_NOT_INCLUDED;
#else
    if (run_and_compare("basename a/b/f", "f\n", 2))
	return TEST_RES_FAIL;
    if (run_and_compare("basename zzz", "zzz\n", 4))
	return TEST_RES_FAIL;
    return TEST_RES_SUCCESS;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("basename", fn_basename, 0);
#endif
