#ifdef TEST_SOURCE
#include "test_tool.h"


const char grep_file[] = "1\n2 Hello\n3 Bye\n4 Bye Bye\n5\n6";
const char grep_file1[] = "yes\nno";

const char grep_[] = "2 Hello\n";
const char grep_H[] = "g:2 Hello\n";
const char grep_n[] = "2:2 Hello\n";
const char grep_q[] = "";
const char grep_v[] = "no\n";
const char grep_s[] = "";
const char grep_c[] = "g:2\ng1:0\n";

const char grep_A[] = "2 Hello\n3 Bye\n";
const char grep_B[] = "1\n2 Hello\n";
const char grep_C[] = "1\n2 Hello\n3 Bye\n";

int fn_grep(void)
{
#ifndef BB_GREP
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;
    char *e = NULL;
    int l = 0;

    write_file("g", grep_file, sizeof(grep_file), 1, 0);
    write_file("g1", grep_file1, sizeof(grep_file1), 1, 0);

    RUN_COMPARE("grep Hello g", grep_);
    RUN_COMPARE("grep -i eLLo g", grep_);
    RUN_COMPARE("grep -H Hello g", grep_H);
    RUN_COMPARE("grep -n Hello g", grep_n);
    RUN_COMPARE("grep -v yes g1", grep_v);
    RUN_COMPARE("grep -q yes g1", grep_q);
    RUN_COMPARE("grep -c Bye g g1", grep_c);
    activate_e("grep -s Bye no_such_file", NULL, &e, &l, NULL, NULL);
    free(e);
    if (l)
	goto Exit;
    RUN_COMPARE("grep -A 1 Hello g", grep_A);
    RUN_COMPARE("grep -B 1 Hello g", grep_B);
    RUN_COMPARE("grep -C 1 Hello g", grep_C);

    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("grep", fn_grep, 1);
#endif
