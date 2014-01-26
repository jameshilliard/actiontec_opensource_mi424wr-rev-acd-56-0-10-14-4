#ifdef TEST_SOURCE
#include "test_tool.h"

#define T_TAR_SIZE 6144

char *tar_out= "a/\na/b/\na/b/c/\na/b/c/f2\na/f1\na/f2\n";

int fn_tar(void)
{
#ifndef BB_TAR
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;
    int l = strlen(tar_out);
    char *p;
    struct stat st;

    create_path("a/b/c");
    copy_file("../file1.txt",  "a/f1");
    copy_file("../file2.txt",  "a/f2");
    copy_file("a/f2", "a/b/c/f2");
    p = activate("tar cvf t.tar a", &l);
    if (stat("t.tar", &st))
	goto Exit;

    if (l!=strlen(tar_out) || memcmp(p, tar_out, l))
	goto Exit;
    if (st.st_size!=T_TAR_SIZE)
	goto Exit;
    free(p);
    p = NULL;

    remove_recursive("a");
    p = activate("tar xvf t.tar", &l);
    if (l!=strlen(tar_out) || memcmp(p, tar_out, l))
	goto Exit;
    free(p);
    p = NULL;

    if (!check_dir("a"))
	goto Exit;
    if (!check_dir("a/b"))
	goto Exit;
    if (!check_dir("a/b/c"))
	goto Exit;
    
    if (stat("a/f1", &st))
	goto Exit;
    if (stat("a/f2", &st))
	goto Exit;

    if (!verify_file_content("a/b/c/f2", file2_txt, sizeof(file2_txt)))
	goto Exit;

    ret = TEST_RES_SUCCESS;
Exit:
    free(p);
    return ret;
#endif
}
#endif
#ifdef SET_ENTRIES
register_test("tar", fn_tar, 1);
#endif
