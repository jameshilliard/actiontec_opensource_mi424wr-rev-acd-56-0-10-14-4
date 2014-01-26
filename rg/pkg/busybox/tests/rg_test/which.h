#ifdef TEST_SOURCE
#include "test_tool.h"

char which_out[] = "/bin/which\n";

int fn_which(void)
{
#ifndef BB_WHICH
    return TEST_RES_NOT_INCLUDED;
#else
    if (run_and_compare("which which", which_out, sizeof(which_out)-1))
	return TEST_RES_FAIL;
    return TEST_RES_SUCCESS;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("which", fn_which, 0);
#endif
