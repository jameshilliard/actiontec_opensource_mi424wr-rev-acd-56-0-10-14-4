#ifdef TEST_SOURCE
#include "test_tool.h"

enum {
    DF_S = 0,
    DF_K = 1,
    DF_M = 2
};

const char df_s[] = "Filesystem                Size      Used Available Use%"
              " Mounted on\n";
const char df_k[] = "Filesystem           1k-blocks      Used Available Use%"
              " Mounted on\n";
const char df_M[] = "Filesystem           1M-blocks      Used Available Use%"
              " Mounted on\n";

/* return negative at failure */
int df_verify_and_get_values(char *p, int size_type, int l)
{
    int sz0 = sizeof(df_k)-1;
    char *t = p+sz0;
    char *df0;
    int i;
    int ret = 0;

    if (l < sz0+12)
	return -1;

    if (size_type == DF_S)
	df0 = (char*)df_s;
    else if (size_type == DF_K)
	df0 = (char*)df_k;
    else
	df0 = (char*)df_M;

    if (memcmp(p, df0, sz0))
	return -1;

    for(i=0; i<2; i++)
	t = skip_begining_of_next_word(t);

    if (size_type==DF_K)
	ret = atoi(t);

    for (i=0; i<3; i++)
	t = skip_begining_of_next_word(t);

    for (; *t!='\n'; t++);
    t++;

    if (t-p != l)
	return -1;

    return ret;
}

int fn_df(void)
{
#ifndef BB_DF
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;
    int sb, sa;
    char *p;
    int l;

    l = 2*sizeof(df_k)-2;
    p = activate("df .", &l);
    sb = df_verify_and_get_values(p, DF_K, l);
    free(p);
    if (sb < 0)
	goto Exit;
    
    write_file("10k", "T", 1, 1024*10, 0); 

    l = 2*sizeof(df_k)-2;
    p = activate("df .", &l);
    sa = df_verify_and_get_values(p, DF_K, l);
    free(p);
    if (sa != sb+10)
	goto Exit;
    
    l = 2*sizeof(df_s)-2;
    p = activate("df -h .", &l);
    sa = df_verify_and_get_values(p, DF_S, l);
    free(p);
    if (sa < 0)
	goto Exit;
	
    l = 2*sizeof(df_M)-2;
    p = activate("df -m .", &l);
    sa = df_verify_and_get_values(p, DF_M, l);
    free(p);
    if (sa < 0)
	goto Exit; 

    ret = TEST_RES_SUCCESS;
    
Exit:
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("df", fn_df, 1);
#endif
