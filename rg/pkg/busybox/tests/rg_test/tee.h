#ifdef TEST_SOURCE
#include "test_tool.h"

#define VERIFY_FILE_CONTENT(name, buf, len) \
    if (!verify_file_content(name, buf, len)) \
        goto Exit;

int fn_tee(void)
{
#ifndef BB_TEE
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;
    int ol, il;
    char *p = NULL;
    int sz0 = 10;
    int sz1 = sizeof(file1_txt)-sz0;
    char *f2 = file1_txt+sz0;
    
    ol = il = sz0;
    p = activate_e("tee f1 f11", &ol, NULL, NULL, file1_txt, &il);
    if (ol!=sz0 || memcmp(p, file1_txt, ol))
	goto Exit;
    free(p);
    VERIFY_FILE_CONTENT("f1", file1_txt, sz0);
    VERIFY_FILE_CONTENT("f11", file1_txt, sz0);
    
    ol = il = sz1;
    p = activate_e("tee -a f1 f11", &ol, NULL, NULL, f2, &il);

    if (ol!=sz1 || memcmp(p, f2, ol))
	goto Exit;
    free(p);
    
    VERIFY_FILE_CONTENT("f1", file1_txt, sizeof(file1_txt));
    VERIFY_FILE_CONTENT("f11", file1_txt, sizeof(file1_txt));

    ret = TEST_RES_SUCCESS;
Exit:
    free(p);
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("tee", fn_tee, 1);
#endif
