#ifdef TEST_SOURCE
#include "test_tool.h"

const char date_out[] = "Wed Jan 16 13:54:2? ??? 1974\n";

int fn_date(void)
{
#ifndef BB_DATE
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;
    char *p;
    int l, el, i;

    l = el = sizeof(date_out)-1;
    p = activate("date 011613541974.20", &l);
    if (l != el)
	goto Exit;
    for (i=0; i<l; i++)
	if (date_out[i] != '?' && date_out[i] != p[i])
	    goto Exit;
    free(p);
    p = NULL;

    RUN_COMPARE("date -R", "Wed, 16 Jan 1974 13:54:20 \n");
    
    p = activate("date", &l);
    if (l != el)
	goto Exit;
    for (i=0; i<l; i++)
	if (date_out[i] != '?' && date_out[i] != p[i])
	    goto Exit;
    
    ret = TEST_RES_SUCCESS;
Exit:
    free(p);
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("date", fn_date, 0);
#endif
