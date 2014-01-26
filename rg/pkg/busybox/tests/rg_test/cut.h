#ifdef TEST_SOURCE
#include "test_tool.h"

const char cut_file[] = "111 222 333 444 555 666\n"
                  "aaa bbb ccc ddd eee fff\n"
		  "one two three four five six\n"
		  "1234567890";
#define CUT_DF "111 222\naaa bbb\none two\n1234567890"
#define CUT_SDF1 "555 666\neee fff\nfive six\n"
#define CUT_SDF2 "111 222 333\naaa bbb ccc\none two three\n"
#define CUT_C "11 \naa \nne \n234\n"
		  
int fn_cut(void)
{
#ifndef BB_CUT
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;

    write_file("c", cut_file, sizeof(cut_file), 1, 0);

    RUN_COMPARE("cut -d @SPACE@ -f -2 c", CUT_DF);
    RUN_COMPARE("cut -s -d @SPACE@ -f 5- c", CUT_SDF1);
    RUN_COMPARE("cut -s -d @SPACE@ -f 1-3 c", CUT_SDF2);
    RUN_COMPARE("cut -c 2-4 c", CUT_C);
    
    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("cut", fn_cut, 1);
#endif
