#ifdef TEST_SOURCE
#include "test_tool.h"

int fn_usleep(void)
{
#ifndef BB_SLEEP
    return TEST_RES_NOT_INCLUDED;
#else
    
    struct timeval be, af;
    long diff;

    gettimeofday(&be, NULL);
    activate("usleep 1700000", NULL);
    gettimeofday(&af, NULL);
    diff = time_diff(&be, &af);
    
    diff -= 1700000;
    
    if (diff>SLEEP_ERROR_TIME || diff<0)
	return TEST_RES_FAIL;
	
    return TEST_RES_SUCCESS;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("usleep", fn_usleep, 0);
#endif
