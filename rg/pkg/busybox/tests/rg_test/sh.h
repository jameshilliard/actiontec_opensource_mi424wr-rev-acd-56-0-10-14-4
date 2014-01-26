#ifdef TEST_SOURCE
#include <unistd.h>
#include <string.h> 

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "test_tool.h"

const char source[] = 
"export ZZZ=zzz\necho $ZZZ\ncat ../file1.txt | tail -n 3 | head -n 1\n"
"cat ../file1.txt > f1\npwd\nmkdir a\ncd a\npwd\n$TEST_TOOL -u 70000 | cat &\n"
"jobs\n";

const char src_out0[] = "zzz\n        advancements in broadband technology.\n";
const char src_out1[] = "/a\n[1] ";
const char src_out2[] = "\n[1] Running                ";
const char src_out3[] = " -u 70000 | cat &";

const char script[] = "#!/bin/sh\necho 123\n";

#define CMP_AND_ADVANCE(b) \
{\
    if(strncmp(t, b, strlen(b))) \
	goto Exit; \
    t += strlen(b); \
}

/* in BB sh.c job test printing format is limited to 40 chars */
#define MAX_JOB_TEST_LEN 40

int fn_sh(void)
{
#ifndef BB_SH
    return TEST_RES_NOT_INCLUDED;
#else
    char *p = NULL;
    char *t;
    int lmax = sizeof(src_out0)+sizeof(src_out1)+sizeof(src_out2)+
	sizeof(src_out3)+MAX_PATH_SIZE*3+9;
    int l = lmax;
    char *cdr = NULL;
    int ret = TEST_RES_FAIL;
    char *job_text = NULL;
    int job_text_len = 0;

    if(setenv("TEST_TOOL", test_tool_full_path, 1))
	end_program(1, "cannot set environment variable");

    write_file("source", source, strlen(source), 1, 0);

    t = p = activate("sh source", &l);
    cdr = x_malloc(MAX_PATH_SIZE);
    if(!getcwd(cdr, MAX_PATH_SIZE))
	end_program(1, "cannot get current directory");
    if(l>lmax || l<2*strlen(cdr))
	goto Exit;

    CMP_AND_ADVANCE(src_out0);
    CMP_AND_ADVANCE(cdr);
    t++;
    CMP_AND_ADVANCE(cdr);
    CMP_AND_ADVANCE(src_out1);
    while(*t!='\n')
	t++;
    CMP_AND_ADVANCE(src_out2);

    job_text = x_malloc(MAX_PATH_SIZE+1);
    strncpy(job_text, test_tool_full_path, MAX_JOB_TEST_LEN);
    job_text[MAX_JOB_TEST_LEN] = 0;
    job_text_len = MAX_JOB_TEST_LEN-strlen(job_text);
    strncat(job_text, src_out3, job_text_len);
    CMP_AND_ADVANCE(job_text);
    t++;
    
    if (t-p!=l)
	goto Exit;
    free(cdr);
    cdr = NULL;
    free(job_text);
    job_text = NULL;
    free(p);
    
    if (!verify_file_content("f1", file1_txt, sizeof(file1_txt)))
	goto Exit;

    write_file("script.sh", script, strlen(script), 1,
	S_IRUSR|S_IRGRP|S_IROTH|S_IXUSR|S_IXGRP|S_IXOTH);
    
    l = 4;
    p = activate("./script.sh", &l);
    if (l!=4 || memcmp(p, "123\n", l))
	goto Exit;

    ret = TEST_RES_SUCCESS;

Exit: 
    free(job_text);
    free(cdr);
    free(p);
    return ret;
#endif
}
#endif
#ifdef SET_ENTRIES
register_test("sh", fn_sh, 1);
#endif
