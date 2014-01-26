#ifdef TEST_SOURCE
#include "test_tool.h"

#define EXPR_TEST(ex, res) \
{ \
    if (run_and_compare("expr "ex, res"\n", strlen(res"\n"))) \
	goto Exit; \
}

int fn_expr(void)
{
#ifndef BB_EXPR
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;

    EXPR_TEST("15 + 17", "32");
    EXPR_TEST("3 \\| ma", "3");
    EXPR_TEST("0 \\| ma", "ma");
    EXPR_TEST("3 \\& ma", "3");
    EXPR_TEST("0 \\& ma", "0");
    EXPR_TEST("0 \\& 0", "0");
    EXPR_TEST("moo \\< hoo", "0");
    EXPR_TEST("123 \\<= 321", "1");
    EXPR_TEST("abcd = abcd", "1");
    EXPR_TEST("abcd != abcd", "0");
    EXPR_TEST("32 \\> 32", "0");
    EXPR_TEST("32 \\>= 32", "1");
    EXPR_TEST("12 + -9", "3");
    EXPR_TEST("2 * 3", "6");
    EXPR_TEST("12 / 3", "4");
    EXPR_TEST("1 + 2 * 3", "7");
    EXPR_TEST("13 % 3", "1");
    EXPR_TEST("dddL : d*L", "4");
    EXPR_TEST("match dddL d*L", "4");
    EXPR_TEST("substr what 2 2", "ha");
    EXPR_TEST("length abcde", "5");
    EXPR_TEST("quote z", "z");
    EXPR_TEST("( 2 + 4 ) / 2", "3");
    
    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("expr", fn_expr, 0);
#endif
