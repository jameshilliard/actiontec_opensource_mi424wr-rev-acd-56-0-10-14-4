#ifdef TEST_SOURCE
#include "test_tool.h"

#define TEMP_GZ_SIZE 1238

int fn_zip_gunzip_zcat(void)
{
#if !(defined (BB_GUNZIP) || defined(BB_GZIP))
    return TEST_RES_NOT_INCLUDED;
#else
    int i;
    int ret = TEST_RES_FAIL;
    int l = 256*sizeof(file1_txt);
    char *p = NULL, *t;
    struct stat st;

    write_file("temp", file1_txt, sizeof(file1_txt), 256, 0);
    activate("gzip temp", NULL);
    
    if (stat("temp.gz", &st))
	goto Exit;
    if (st.st_size!=TEMP_GZ_SIZE)
	goto Exit;
    
    t = p = activate("zcat temp.gz", &l);
    if (l!=256*sizeof(file1_txt))
	goto Exit;

    for (i=0; i<256; i++)
    {
	if (memcmp(t, file1_txt, sizeof(file1_txt)))
	    goto Exit;
	t += sizeof(file1_txt);
    }
    free(p);
    p = NULL;

    activate("gunzip temp.gz", NULL);
    
    t = p = get_file_content("temp", &l);
    if (l!=256*sizeof(file1_txt))
	goto Exit;

    for (i=0; i<256; i++)
    {
	if (memcmp(t, file1_txt, sizeof(file1_txt)))
	    goto Exit;
	t += sizeof(file1_txt);
    }

    ret = TEST_RES_SUCCESS;
Exit:
    free(p);
    return ret;
#endif
}
#endif
#ifdef SET_ENTRIES
register_test("zip_gunzip_zcat", fn_zip_gunzip_zcat, 1);
#endif
