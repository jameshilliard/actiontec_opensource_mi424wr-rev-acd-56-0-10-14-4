#ifdef TEST_SOURCE
#include "test_tool.h"

#define QUOTE "bla bla bla"

int fn_echo(void)
{
#ifndef BB_ECHO
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;

    RUN_COMPARE("echo "QUOTE, QUOTE"\n");
    RUN_COMPARE("echo -n "QUOTE, QUOTE);
    RUN_COMPARE("echo -e "QUOTE" @NEWLINE@ "QUOTE, QUOTE" \n "QUOTE"\n");
    
    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("echo",fn_echo,0);
#endif
