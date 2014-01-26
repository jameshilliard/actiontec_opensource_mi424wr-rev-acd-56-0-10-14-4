#ifdef TEST_SOURCE
#include "test_tool.h"
#include <dirent.h>

int fn_mv(void)
{
#ifndef BB_CP_MV
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;

    copy_file("../file1.txt", "f1");
    activate("mv f1 mf1", NULL);
    if (!verify_file_content("mf1", file1_txt, sizeof(file1_txt)))
	goto Exit;

    create_path("a");
    activate("mv a b", NULL);

    if (check_dir("a"))
	goto Exit;
   
    if (!check_dir("b"))
	goto Exit;

    ret = TEST_RES_SUCCESS;

Exit:
    return ret;
#endif
}
#endif
#ifdef SET_ENTRIES
register_test("mv", fn_mv, 1);
#endif
