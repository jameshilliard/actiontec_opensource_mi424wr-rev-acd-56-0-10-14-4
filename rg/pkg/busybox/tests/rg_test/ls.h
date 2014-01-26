#ifdef TEST_SOURCE
#include "test_tool.h"
#include <unistd.h>

/* Notice: ls prints different output to a tty fd and to a non-tty fd
 * Here we only check the not tty option.
 *
 * Notice: time stamps (as in ls -l) are not checked.
 */


const char ls_l[] = 
"drwxr-xr-x   4 "LS_USER" "LS_GROUP"     "LS_DIRSIZE" MMM DD HH:MM a\n"
"-r--r--r--   2 "LS_USER" "LS_GROUP"      669 MMM DD HH:MM f1_h_link\n"
"lrwxrwxrwx   1 "LS_USER" "LS_GROUP"        9 MMM DD HH:MM f1_s_link -> file1.txt\n"
"-r--r--r--   2 "LS_USER" "LS_GROUP"      669 MMM DD HH:MM file1.txt\n"
"-r--r--r--   1 "LS_USER" "LS_GROUP"      469 MMM DD HH:MM file2.txt\n"
"lrwxrwxrwx   1 "LS_USER" "LS_GROUP"        1 MMM DD HH:MM linked_dir -> a\n";

const char ls[] = "a\nf1_h_link\nf1_s_link\nfile1.txt\nfile2.txt\nlinked_dir\n";

const char ls_a[] = ".\n..\n.hidden\na\nf1_h_link\nf1_s_link\nfile1.txt\n"
    "file2.txt\nlinked_dir\n";

const char ls_A[] = ".hidden\na\nf1_h_link\nf1_s_link\nfile1.txt\nfile2.txt\n"
    "linked_dir\n";

const char ls_R[] = ".:\na\nf1_h_link\nf1_s_link\nfile1.txt\nfile2.txt\nlinked_dir\n"
    "\n./a:\nb\nb2\n\n./a/b:\nc\n\n./a/b/c:\nfile2.txt\n\n./a/b2:\n";

const char ls_s[] = "    1 a\n    1 f1_h_link\n    0 f1_s_link\n    1 file1.txt\n"
    "    1 file2.txt\n    0 linked_dir\n";

const char ls_rt[] = "a\nfile1.txt\nf1_h_link\nfile2.txt\nf1_s_link\nlinked_dir\n";


const char ls_t[] = "linked_dir\nf1_s_link\nfile2.txt\nf1_h_link\nfile1.txt\na\n";

const char ls_lL[] = "drwxr-xr-x   4 "LS_USER" "LS_GROUP"     "LS_DIRSIZE" MMM DD HH:MM a\n"
"-r--r--r--   2 "LS_USER" "LS_GROUP"      669 MMM DD HH:MM f1_h_link\n"
"-r--r--r--   2 "LS_USER" "LS_GROUP"      669 MMM DD HH:MM f1_s_link\n"
"-r--r--r--   2 "LS_USER" "LS_GROUP"      669 MMM DD HH:MM file1.txt\n"
"-r--r--r--   1 "LS_USER" "LS_GROUP"      469 MMM DD HH:MM file2.txt\n"
"drwxr-xr-x   4 "LS_USER" "LS_GROUP"     "LS_DIRSIZE" MMM DD HH:MM linked_dir\n";

const char ls_d[] = "a\n";

int cmp_ignore_time_stamp(char *result,const char *prepared, int buf_len)
{
    char *rs = result, *pr = (char *)prepared;
    char *r = rs, *p = pr;
    int i;
    
    while(rs-result<buf_len && *pr)
    {
	r = rs;
	p = pr;
	/* find begining of date field */
	for(i=0; i<5; i++)
	{
	    pr = skip_begining_of_next_word(pr);
	    rs = skip_begining_of_next_word(rs);
	}

	for(;p<pr && r<rs; r++,p++)
	    if (*p!=*r)
		return 1;
	
	/* skip date stuff */
	for(i=0; i<3; i++)
	{
	    pr = skip_begining_of_next_word(pr);
	    rs = skip_begining_of_next_word(rs);
	}

	r = rs;
	p = pr;
	for(; *pr!='\n' && *pr && *rs!='\n' && rs-result<buf_len; rs++,pr++)
	    if (*pr!=*rs)
		return 1;
	if (*pr=='\n' && *rs!='\n')
	    return 1;
	if (!*pr && rs-result!=buf_len)
	    return 1;

	pr++;
	rs++;
    }
    return 0;
}

#define LS_CMP_NOTIME_TEST(cmd, buf) \
{ \
    l = sizeof(buf); \
    p = activate(cmd, &l); \
    if (cmp_ignore_time_stamp(p, buf, l)) \
	goto Exit; \
    free(p); \
    p = NULL; \
}

#define LS_CMP_TEST(cmd, buf) \
{ \
    l = sizeof(buf)-1; \
    p = activate(cmd, &l); \
    if (l!=sizeof(buf) && memcmp(p, buf, l)) \
	goto Exit; \
    free(p); \
    p = NULL; \
}

int fn_ls(void)
{
#ifndef BB_LS
    return TEST_RES_NOT_INCLUDED;
#else
    int ret = TEST_RES_FAIL;
    char *p = NULL;
    int l;

    /* preparing the files and directories structure
     * sleep()'s are needed for timestamps differences
     */
    create_path("a/b/c");
    create_path("a/b2");
    sleep(1);
    copy_file("../file1.txt", "file1.txt");
    sleep(1);
    copy_file("../file2.txt", "file2.txt");
    sleep(1);
    symlink("file1.txt", "f1_s_link");
    sleep(1);
    link("file1.txt", "f1_h_link");
    sleep(1);
    symlink("file2.txt", "a/b/c/file2.txt");
    copy_file("file2.txt", ".hidden");
    sleep(1);
    symlink("a", "linked_dir");

    /* performing the tests: */
    LS_CMP_NOTIME_TEST("ls -l", ls_l);
    LS_CMP_NOTIME_TEST("ls -lL", ls_lL);
    
    LS_CMP_TEST("ls", ls);
    LS_CMP_TEST("ls -a", ls_a);
    LS_CMP_TEST("ls -A", ls_A);
    LS_CMP_TEST("ls -R", ls_R);
    LS_CMP_TEST("ls -1", ls);
    LS_CMP_TEST("ls -s", ls_s);
    LS_CMP_TEST("ls -r -t", ls_rt);
    LS_CMP_TEST("ls -t", ls_t);
    LS_CMP_TEST("ls -d a", ls_d);

    ret = TEST_RES_SUCCESS;
Exit:
    free(p);
    return ret;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("ls", fn_ls, 1);
#endif
