#ifdef TEST_SOURCE
#include "test_tool.h"
#include <unistd.h>

int fn_cp(void)
{
#ifndef BB_CP_MV
    return TEST_RES_NOT_INCLUDED;
#else
    struct stat st, sts;
    int ret = TEST_RES_FAIL;

    activate("cp ../file1.txt a", NULL);
    if (!verify_file_content("a", file1_txt, sizeof(file1_txt)))
	goto Exit;
	    
    symlink("a", "link_a");
    activate("cp -d link_a link_b", NULL);
    if(!verify_link("link_b", "a"))
	goto Exit;

    create_path("aa/bb/cc");
    create_path("aa/bb/CC");
    activate("cp ../file2.txt aa/bb", NULL);
    activate("cp aa/bb/file2.txt aa/bb/CC/file", NULL);
    if (stat("aa/bb/file2.txt", &st) || stat("aa/bb/CC/file", &st))
	goto Exit;
    activate("cp -R aa newaa", NULL);
    if (stat("newaa", &st) || stat("newaa/bb", &st) || stat("newaa/bb/cc", &st)
	|| stat("newaa/bb/CC", &st) || stat("newaa/bb/file2.txt", &st)
	|| stat("newaa/bb/CC/file", &st))
    {
	goto Exit;
    }

    usleep(50000);
    activate("cp -p a s", NULL);
    memset(&st, 0, sizeof(st));
    memset(&sts, 0, sizeof(sts));
    if (stat("a", &st) || stat("s", &sts))
	goto Exit;
    
    st.st_ino = sts.st_ino;
    st.st_rdev = sts.st_rdev;
    st.st_atime = sts.st_atime;
    st.st_ctime = sts.st_ctime;
    if (memcmp(&st, &sts, sizeof(st)))
	goto Exit;
    
    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("cp", fn_cp, 1);
#endif
