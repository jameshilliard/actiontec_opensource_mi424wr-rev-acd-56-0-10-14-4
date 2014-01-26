#ifdef TEST_SOURCE
#include "test_tool.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

const char ln_errmsg[] = "ln: a: ";

#define LN_F1 "../file1.txt"

int fn_ln(void)
{
#ifndef BB_LN
    return TEST_RES_NOT_INCLUDED;
#else
    struct stat stb, stc;
    int ret = TEST_RES_FAIL;
    char *err = NULL;
    char *e = strerror(EEXIST);
    int erl1 = strlen(ln_errmsg);
    int erl2 = strlen(e);
    int errlen = erl1+erl2+1;

    activate("ln -s "LN_F1" a", NULL);
    if (!verify_link("a", LN_F1))
	goto Exit;
    copy_file(LN_F1, "b");
    
    activate_e("ln -s b a", NULL, &err, &errlen, NULL, NULL);

    /* check that a is still a link and that it HAS NOT chenged */
    if (!verify_link("a", LN_F1))
	goto Exit;

    if (errlen!=erl1+erl2+1 || strncmp(err, ln_errmsg, erl1) ||
	strncmp(err+erl1, e, erl2) || *(err+errlen-1) != '\n' )
    {
	free(err);
	goto Exit;
    }
    free(err);
    
    activate("ln -s -f b a", NULL);

    /* check that a is still a link and that it HAS chenged */
    if (!verify_link("a", "b"))
	goto Exit;

    /* check hard link - see that files have same properties */
    /* memsets are needed to avoid garbage left in pading bytes */
    memset(&stb, 0, sizeof(stb));  
    memset(&stc, 0, sizeof(stc));
    activate("ln b c", NULL);
    if (stat("c", &stc) || stat("b", &stb))
	goto Exit;

    if (memcmp(&stc, &stb, sizeof (struct stat)))
	goto Exit;

    ret = TEST_RES_SUCCESS;
Exit:    
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("ln", fn_ln, 1);
#endif
