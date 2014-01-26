#ifdef TEST_SOURCE
#include "test_tool.h"

int fn_sleep(void)
{
#ifndef BB_SLEEP
    return TEST_RES_NOT_INCLUDED;
#else
    struct timeval be, af;
    long diff;

    gettimeofday(&be, NULL);
    activate("sleep 1", NULL);
    gettimeofday(&af, NULL);
    diff = time_diff(&be, &af)-1000000;

    if (diff>SLEEP_ERROR_TIME || diff<0)
    {
	return TEST_RES_FAIL;
    }
    
    return TEST_RES_SUCCESS;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("sleep", fn_sleep, 0);
#endif
