#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/syslog.h>
#include <stdarg.h>

#include "test_tool.h"

char *prog_name;
char *test_tool_full_path;
char *test_env_path;
int exit_on_fail;
int msg_on_success=1;

struct single_test
{
    char *name;
    int (*fn)(void); 
    int mkdir;               /* 1 if test needs a directory of its own */
    struct single_test *next;
};

struct single_test *test_list_head;

void end_program(int rval, const char *fmt, ...)
{
    va_list ptr;
    
    va_start(ptr, fmt);

    if (rval!=0)
    {
	fprintf(stderr, "%s exiting, test not completed <", prog_name);
	fprintf(stderr, fmt, ptr);
	fprintf(stderr, ">\n");
    }
    else if (*fmt)
    {
	printf("%s: ", prog_name);
	vprintf(fmt, ptr);
	printf("\n");
    }

    while (test_list_head)
    {
	struct single_test *f = test_list_head;
	test_list_head = f->next;
	free(f);
    }

    va_end(ptr);

    free(test_tool_full_path);
    free(test_env_path);

    exit(rval);
}

void signal_handler(int sig)
{
    char name[4];
    sprintf(name, "%d", sig);
    write_file(name, name, strlen(name), 1, 0);
    end_program(0, "");
}
void usage()
{
    fprintf(stderr,
        "Usage: %s "
	"[ [-p path] [-s] [-f] [ <test1> <test2> ...]] |"
	"-l <message for log> | -a\n", prog_name );
    fprintf(stderr,"-l\tprint anything after the -l to system log\n");
    fprintf(stderr,"-a\tlist all included tests and exit\n");
#if 0
    /* information only, not for user's command line */
    fprintf(stderr,"-u mils\t sleep for mils milliseconds and exit\n");
    fprintf(stderr,"-g sig\tupon accepting signal sig, write a file "
	    "named sig print sig and quit\n");
#endif
    fprintf(stderr,"-f\texit upon test failure\n");
    fprintf(stderr,"-s\tdo not print success messages\n");
    fprintf(stderr,"-p path\tadd path (of BB links).\n");
    fprintf(stderr,"test1 test2 ...\t names of BusyBox applets to test\n");

    end_program(0, "");
}

char *x_malloc(int size)
{
    char *p = (char*)malloc(size);

    if (!p)
	end_program(1, "cannot malloc");
    return p;
}

void set_bb_path(char *path)
{
    char *buf;

    if (*path != '/')
    {
	buf = x_malloc(MAX_PATH_SIZE);
	if (!getcwd(buf, MAX_PATH_SIZE-1-strlen(path)))
	    end_program(1, "could not get current directory");
	strcat(buf, "/");
	strcat(buf, path);
    }
    else
	buf = path;
	
    if (setenv("PATH", buf, 1))
	end_program(1, "setenv failed");

    if (buf != path)
	free(buf);
}

void register_test(char * name, int (*fn)(void), int mkdir)
{
    struct single_test *t = (struct single_test *)
	x_malloc(sizeof(struct single_test)+1+strlen(name));
    char *namep = (char*)t+sizeof(struct single_test);

    strcpy(namep, name);
    t->name = namep;
    t->fn = fn;
    t->mkdir = mkdir;

    t->next = test_list_head;
    test_list_head = t;
}

#define DECL_FILE_BUFFER
#include "all_inputs.h" /* get files content buffers */
#undef DECL_FILE_BUFFER
#define TEST_SOURCE
#include "all_tests.h"  /* get functions defenitions */
#undef TEST_SOURCE

void set_entries()
{
#define SET_ENTRIES
#include "all_tests.h"  /* register all tests */
#undef SET_ENTRIES
}

void write_to_log(int argc, char **argv)
{
    int i,len = 0;
    char *msg;

    for (i=0; i<argc; i++)
	len += 1+strlen(argv[i]);

    if (!len)
	usage();
    
    msg = x_malloc(len);

    for (i=0, len=0; i<argc; i++)
    {
	strcpy(msg+len, argv[i]);
	len+=strlen(argv[i]);
	msg[len++] = ' ';
    }

    msg[len-1] = 0;

    openlog(prog_name, 0, LOG_USER);
    syslog(LOG_INFO, "%s", msg);
    closelog();
    
    free(msg);
    end_program(0, "");
}

struct single_test *find_single_test(char *name)
{
    struct single_test *t;

    if (!name)
	return NULL;
    
    for (t=test_list_head; t; t=t->next)
	if (!strcmp(t->name, name))
	    return t;
    return NULL;
}


void prepare_environment()
{
    struct stat st;
    
    remove_recursive(ENV_DIR);
    if (!stat(ENV_DIR, &st))
	end_program(1, "could not remove existing " ENV_DIR);

    test_tool_full_path = x_malloc(MAX_PATH_SIZE);
    if (*prog_name=='/')
	strcpy(test_tool_full_path, prog_name);
    else
    {
	if (!getcwd(test_tool_full_path, MAX_PATH_SIZE-1-strlen(prog_name)))
	    end_program(1, "cannot get curren working dir");
	strcat(test_tool_full_path, "/");
	strcat(test_tool_full_path, prog_name);
    }

    if (mkdir(ENV_DIR, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH))
	end_program(1, "could not mkdir " ENV_DIR);
    if (chdir(ENV_DIR))
	end_program(1, "could not change dir to " ENV_DIR);
    test_env_path = x_malloc(MAX_PATH_SIZE);
    if (!getcwd(test_env_path, MAX_PATH_SIZE))
	end_program(1, "coud not get current working dir");
 
/* write neccesary files: (input files for testing procedure) */
#define WRITE_FILE
#include "all_inputs.h"
#undef WRITE_FILE
}

void execute_single_test(struct single_test *t, char *name)
{
    int ret;
    
    if (!t)
	t = find_single_test(name);
    if (!t)
    {
	fprintf(stderr, "could not find test <%s>\n", name);
	return;
    }
    
    if (t->mkdir)
    { 
	if (mkdir(t->name, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)) 
	    end_program(1, "could not mkdir"); 
	if (chdir(t->name)) 
	    end_program(1, "could not change dir"); 
    }

    printf("%s: ", t->name);
    fflush(stdout);
    ret = t->fn();
    if (t->mkdir)
	chdir(test_env_path);
    switch (ret)
    {
	case TEST_RES_NOT_INCLUDED:
	    fprintf(stdout,"APP NOT IN BB\n");
	    break;
	case TEST_RES_SUCCESS:
	    if (msg_on_success)
		fprintf(stdout,"SUCCESS\n");
	    if (t->mkdir)
		remove_recursive(t->name);
	    break;
	case TEST_RES_FAIL:
	    fprintf(stdout, "FAIL\n");
	    if (exit_on_fail)
		end_program(0, "test failed - exiting");
	    break;
	default:
	    end_program(1, "invalid test result");
    }
}

int main(int argc, char **argv)
{
    int i;
    struct single_test *t;
    int list_tests = 0;
    char **arg = argv+1;

    prog_name = argv[0];
   
    for (i=1,arg=argv+i; i<argc && **arg == '-'; i++,arg=argv+i)
    {
	if ((*arg)[2] && !isspace((*arg)[2]))
	    usage();
	switch ((*arg)[1])
	{
	    case '?':
		usage();
		break;
	    case 'a':
		list_tests = 1;
		break;
	    case 'g':
		signal(atoi(argv[i+1]), signal_handler);
		i++;
		break;
	    case 'u':
		usleep(atoi(argv[i+1]));
		end_program(0, "");
	    case 'l':
		write_to_log(argc-i-1, arg+1);
		end_program(0, "");
		break;
	    case 's':
		msg_on_success = 0;
		break;
	    case 'f':
		exit_on_fail = 1;
		break;
	    case 'p':
		set_bb_path(argv[i+1]);
		i++;
		break;
	    default :
		usage();
	}
    }

    prepare_environment();
    set_entries();

    if (list_tests)
    {
	fprintf(stdout, "%s: Tests included: \n", prog_name);
	for (t=test_list_head; t; t=t->next)
	    fprintf(stdout, "%s\n", t->name);
	end_program(0, "Done.");
    }
    
    if (i==argc)  /* no explicit tests specified => execute all */
	for (t=test_list_head; t; t=t->next)
	    execute_single_test(t, NULL);
    else
	for (; i<argc; i++)
	    execute_single_test(NULL, argv[i]);

    end_program(0, "Done.");
}
