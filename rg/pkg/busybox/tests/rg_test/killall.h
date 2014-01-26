#ifdef TEST_SOURCE
#include "test_tool.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>

static int killall_last_sig;

void killall_signal_handler(int sig)
{
   killall_last_sig = sig;
}

int killall_test_sig(int s)
{
    char cmd[128];
    void (*f)(int);
    char *t, *p;
    pid_t pid;
    struct stat st;

    for (t = p = prog_name; *t; t++)
	if (*t=='/')
	    p = t+1;
    
    f = signal(s, killall_signal_handler);
    if(f==SIG_ERR)
	end_program(1, "cannot set signal handler");

    sprintf(cmd, "%s -g %d -u 10000000", test_tool_full_path, s);
    pid = run_program(cmd, NULL, NULL, NULL, NULL, NULL, NULL);
    usleep(1000);

    sprintf(cmd, "killall -%d %s", s, p);
    activate(cmd, NULL);
    waitpid(pid, NULL, 0);
    f = signal(s, f);
    if(f==SIG_ERR)
	end_program(1, "cannot set signal handler back");
    
    if (killall_last_sig!=s)
	return 1;
    killall_last_sig = 0;
    
    usleep(1000);
    sprintf(cmd, "%d", s);
    if (stat(cmd, &st))
	return 1;
    
    return 0;
}

int fn_killall(void)
{
#ifndef BB_KILLALL
    return TEST_RES_NOT_INCLUDED;
#else
    if(killall_test_sig(SIGUSR1))
	return TEST_RES_FAIL;
    return TEST_RES_SUCCESS;
#endif
}

#endif
#ifdef SET_ENTRIES
register_test("killall", fn_killall, 1);
#endif
