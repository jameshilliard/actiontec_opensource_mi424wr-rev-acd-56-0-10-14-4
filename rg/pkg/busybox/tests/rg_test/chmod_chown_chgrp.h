#ifdef TEST_SOURCE
#include "test_tool.h"

int fn_chmod_chown_chgrp(void)
{
#ifndef BB_CHMOD_CHOWN_CHGRP
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;
    int fd;
    struct stat st;
    mode_t mode;

    fd = open("f", O_RDONLY|O_CREAT);
    close(fd);
    
    /* chmod: */
    activate("chmod a+r f", NULL);
    activate("chmod a+w f", NULL);
    activate("chmod a+x f", NULL);
    stat("f", &st);
    mode = S_IRUSR|S_IWUSR|S_IXUSR|S_IRWXG|S_IRGRP|S_IWGRP|S_IXGRP|S_IRWXO|
        S_IROTH|S_IWOTH|S_IXOTH;
    if ((st.st_mode&mode) != mode)
	goto Exit;
    
    activate("chmod o-w f", NULL);
    activate("chmod g-x f", NULL);
    mode = S_IRUSR|S_IWUSR|S_IXUSR|S_IRWXG|S_IRGRP|S_IWGRP|S_IRWXO|
	S_IROTH|S_IXOTH;
    if ((st.st_mode&mode) != mode)
	goto Exit;

    /* chown, chgrp: */
    activate("chown 37 f", NULL);
    activate("chgrp 19 f", NULL);
    stat("f", &st);
    if (st.st_uid != 37)
	goto Exit;
    if (st.st_gid != 19)
	goto Exit;
    
    activate("chown root f", NULL);
    activate("chgrp root f", NULL);
    stat("f", &st);
    if (st.st_uid != 0)
	goto Exit;
    if (st.st_gid != 0)
	goto Exit;

    ret = TEST_RES_SUCCESS;
Exit:
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("chmod_chown_chgrp", fn_chmod_chown_chgrp, 1);
#endif
