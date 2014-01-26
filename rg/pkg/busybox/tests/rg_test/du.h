#ifdef TEST_SOURCE
#include "test_tool.h"
#include "ctype.h"

const char du_[] = "1\t./a/b/c\n3\t./a/b\n1\t./a/b2\n5\t./a\n7\t.\n";
const char du_s[] = "summary\n7\t.\n";
const char du_sh[] = "summary\n7.0k\t.\n";
const char du_sm[] = "summary\n0\t.\n";
const char du_shl[] = "summary\n8.0k\t.\n";

int fn_du(void)
{
#ifndef BB_DU
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;
    char *t, *p = NULL;
    int l;
    int before, after;

    create_path("a/b/c");
    create_path("a/b2");
    copy_file("../file1.txt", "file1.txt");
    copy_file("../file2.txt", "a/b/file2.txt");
    link("file1.txt", "f1_hl");
    
    RUN_COMPARE("du", du_);
    RUN_COMPARE("du -s", du_s);
    RUN_COMPARE("du -sm", du_sm);
    RUN_COMPARE("du -sh", du_sh);
    RUN_COMPARE("du -shl", du_shl);

    l = 64;
    p = activate("du -s", &l);
    for(t = p+8; t-p<64 && !isspace(*t); t++);
    *t = 0;
    before = atoi(p+8);
    free(p);
    write_file("10k", "T", 1, 1024*10, 0);
    l = 64;
    p = activate("du -s", &l);
    for(t = p+8; t-p<64 && !isspace(*t); t++);
    *t = 0;
    after = atoi(p+8);
    free(p);

    if (before!=after-10)
	goto Exit;

    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("du", fn_du, 1);
#endif
