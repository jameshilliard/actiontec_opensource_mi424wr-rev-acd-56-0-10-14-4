#ifdef TEST_SOURCE
#include "test_tool.h"
#include <signal.h>

static int kill_last_sig;

void kill_signal_handler(int sig)
{
   kill_last_sig = sig;
}

int test_sig(int s)
{
    char cmd[32];
    void (*f)(int);
    
    f = signal(s, kill_signal_handler);
    if(f==SIG_ERR)
	end_program(1, "cannot set signal handler");
    sprintf(cmd, "kill -%d %d", s, getpid());
    activate(cmd, NULL);
    f = signal(s, f);
    if(f==SIG_ERR)
	end_program(1, "cannot set signal handler back");

    if (kill_last_sig != s)
	return TEST_RES_FAIL;
    kill_last_sig = 0;
    
    return TEST_RES_SUCCESS;
}

int fn_kill(void)
{
#ifndef BB_KILL
    return TEST_RES_NOT_INCLUDED;
#else
    return test_sig(SIGUSR1);
#endif
}

#endif
#ifdef SET_ENTRIES 
register_test("kill",fn_kill,0);
#endif
