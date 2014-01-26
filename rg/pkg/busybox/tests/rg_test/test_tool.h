#ifndef TEST_TOOL_H
#define TEST_TOOL_H
#include <sys/types.h> 
#include "../../Config.h"  /* get BB Config.h to see which
			    *  applets are included 
			    */

char *get_file_content(const char *name, int *len);
char *activate(const char *cmd, int *out_len);
char *activate_e(const char *cmd, int *outlen, char **errbuf, int *errlen, 
    char *inbuf, int *inlen);
pid_t run_program(const char *cmd, int *fd_stdin, int *fd_stdout,
    int *fd_stderr, int *ci, int *co, int *ce);
char *x_malloc(int size);
char *skip_begining_of_next_word(char* w);
void write_file(const char *name, const char *content, int len, int times,
    int flags);
int run_and_compare(const char *command, const char *buf, int len);
int check_dir(const char *name);
void end_program(int rval, const char *fmt, ...) __attribute__ ((__noreturn__));
int verify_link(char *link, char *name);
long time_diff(struct timeval *a, struct timeval *b);
int verify_file_content(char *name, char *buf, int len);

/* internal utilities to replace usage of BB applets while testing others */
void remove_recursive(char *dir); /* same effect as rm -rf */
void create_path(char *path); /* same effect as mkdir -p */
void copy_file(char *src, char *dst); /* same effect as cp */

typedef enum {
    TEST_RES_SUCCESS = 0,
    TEST_RES_FAIL = 1, 
    TEST_RES_NOT_INCLUDED = 2,
} test_result;

#define RUN_COMPARE(c, b) \
{ \
    char *pb = (char*)b; \
    if (run_and_compare(c, pb, strlen(pb))) \
	goto Exit; \
}

#define MAX_PATH_SIZE 128

#define ENV_DIR "test_env_dir"

/* environment specific definitions: 
 * if you change this, make sure you leave total len the same 
 */
#define LS_USER "0       "
#define LS_GROUP "root    "
#define LS_DIRSIZE "1024"

#define SLEEP_ERROR_TIME 250000  /* maximum time of execution, return and 
				  * measurement actions, in miliseconds
				  */
#define MAX_ARGS 32 /* number of args for activated program */

#define EXTRA_TIME 50 /* time (milliseconds) to wait for program to give extra
			output (probaply unexpected and wrong) */
#define SLOW_TIME 3000 /* time (milliseconds) to wait for program to give
			  expected output */
#endif
