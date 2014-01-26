/* vi: set sw=4 ts=4: */
/*
 * lash -- the BusyBox Lame-Ass SHell
 *
 * Copyright (C) 1999,2000,2001 by Lineo, inc.
 * Written by Erik Andersen <andersen@lineo.com>, <andersee@debian.org>
 *
 * Based in part on ladsh.c by Michael K. Johnson and Erik W. Troan, which is
 * under the following liberal license: "We have placed this source code in the
 * public domain. Use it in any project, free or commercial."
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

/* The parsing engine of this program is officially at a dead-end.
 * Future work in that direction should move to the work posted
 * at http://doolittle.faludi.com/~larry/parser.html .
 * A start on the integration of that work with the rest of sh.c
 * is at http://codepoet.org/sh.c .
 */
//
//This works pretty well now, and is now on by default.
#define BB_FEATURE_SH_ENVIRONMENT
//
//Backtick support has some problems, use at your own risk!
//#define BB_FEATURE_SH_BACKTICKS
//
//If, then, else, etc. support..  This should now behave basically
//like any other Bourne shell -- sortof...
#define BB_FEATURE_SH_IF_EXPRESSIONS
//
/* This is currently sortof broken, only for the brave... */
#undef HANDLE_CONTINUATION_CHARS
//
/* This would be great -- if wordexp wouldn't strip all quoting
 * out from the target strings...  As is, a parser needs  */
#undef BB_FEATURE_SH_WORDEXP
//
//For debugging/development on the shell only...
//#define DEBUG_SHELL

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <getopt.h>

#undef BB_FEATURE_SH_WORDEXP

#if BB_FEATURE_SH_WORDEXP
#include <wordexp.h>
#define expand_t	wordexp_t
#undef BB_FEATURE_SH_BACKTICKS
#else
#include <glob.h>
#define expand_t	glob_t
#endif	
#include "busybox.h"
#include "cmdedit.h"
#include <mmu_context.h>

#ifdef ACTION_TEC_DIFFSERV
static const int _MAX_LINE = 1024;	/* size of input buffer for cwd data */
#else
static const int _MAX_LINE = 256;  /* size of input buffer for cwd data */
#endif //ACTION_TEC_DIFFSERV
static const int MAX_READ = 128;	/* size of input buffer for `read' builtin */
#define JOB_STATUS_FORMAT "[%d] %-22s %.40s\n"

#if !defined (WTERMSIG)
    #define WTERMSIG(s)	      ((s) & 0177)
#endif 
#ifdef ACTION_TEC_DIFFSER
#undef  BUFSIZ
#define BUFSIZ 1024 
#endif //ACTION_TEC_DIFFSERV
enum redir_type { REDIRECT_INPUT, REDIRECT_OVERWRITE,
	REDIRECT_APPEND
};

static const unsigned int DEFAULT_CONTEXT=0x1;
static const unsigned int IF_TRUE_CONTEXT=0x2;
static const unsigned int IF_FALSE_CONTEXT=0x4;
static const unsigned int THEN_EXP_CONTEXT=0x8;
static const unsigned int ELSE_EXP_CONTEXT=0x10;


struct jobset {
	struct job *head;			/* head of list of running jobs */
	struct job *fg;				/* current foreground job */
};

struct redir_struct {
	enum redir_type type;	/* type of redirection */
	int fd;						/* file descriptor being redirected */
	char *filename;				/* file to redirect fd to */
};

struct child_prog {
	pid_t pid;					/* 0 if exited */
	char **argv;				/* program name and arguments */
	int num_redirects;			/* elements in redirection array */
	struct redir_struct *redirects;	/* I/O redirects */
	int is_stopped;				/* is the program currently running? */
	struct job *family;			/* pointer back to the child's parent job */
};

struct job {
	int jobid;					/* job number */
	int num_progs;				/* total number of programs in job */
	int running_progs;			/* number of programs running */
	char *text;					/* name of job */
	char *cmdbuf;				/* buffer various argv's point into */
	pid_t pgrp;					/* process group ID for the job */
	struct child_prog *progs;	/* array of programs in job */
	struct job *next;			/* to track background commands */
	int stopped_progs;			/* number of programs alive, but stopped */
	unsigned int job_context;	/* bitmask defining current context */
	struct jobset *job_list;
};

struct built_in_command {
	char *cmd;					/* name */
	char *descr;				/* description */
	int (*function) (struct child_prog *);	/* function ptr */
};

struct close_me {
	int fd;
	struct close_me *next;
};

/* function prototypes for builtins */
static int builtin_cd(struct child_prog *cmd);
static int builtin_env(struct child_prog *dummy);
static int builtin_exec(struct child_prog *cmd);
static int builtin_exit(struct child_prog *cmd);
static int builtin_fg_bg(struct child_prog *cmd);
static int builtin_help(struct child_prog *cmd);
static int builtin_jobs(struct child_prog *dummy);
static int builtin_pwd(struct child_prog *dummy);
static int builtin_export(struct child_prog *cmd);
static int builtin_source(struct child_prog *cmd);
static int builtin_unset(struct child_prog *cmd);
static int builtin_read(struct child_prog *cmd);
#ifdef BB_FEATURE_SH_IF_EXPRESSIONS
static int builtin_if(struct child_prog *cmd);
static int builtin_then(struct child_prog *cmd);
static int builtin_else(struct child_prog *cmd);
static int builtin_fi(struct child_prog *cmd);
/* function prototypes for shell stuff */
static int run_command_predicate(char *cmd);
#endif


/* function prototypes for shell stuff */
static void mark_open(int fd);
static void mark_closed(int fd);
static void close_all(void);
static void checkjobs(struct jobset *job_list);
static int get_command(FILE * source, char *command);
static int parse_command(char **command_ptr, struct job *job, int *inbg);
static int run_command(struct job *newjob, int inbg, int outpipe[2]);
static int pseudo_exec(struct child_prog *cmd) __attribute__ ((noreturn));
static int busy_loop(FILE * input);


/* Table of built-in functions (these are non-forking builtins, meaning they
 * can change global variables in the parent shell process but they will not
 * work with pipes and redirects; 'unset foo | whatever' will not work) */
static struct built_in_command bltins[] = {
	{"bg", "Resume a job in the background", builtin_fg_bg},
	{"cd", "Change working directory", builtin_cd},
	{"exec", "Exec command, replacing this shell with the exec'd process", builtin_exec},
	{"exit", "Exit from shell()", builtin_exit},
	{"fg", "Bring job into the foreground", builtin_fg_bg},
	{"jobs", "Lists the active jobs", builtin_jobs},
	{"export", "Set environment variable", builtin_export},
	{"unset", "Unset environment variable", builtin_unset},
	{"read", "Input environment variable", builtin_read},
	{".", "Source-in and run commands in a file", builtin_source},
	/* to do: add ulimit */
#ifdef BB_FEATURE_SH_IF_EXPRESSIONS
	{"if", NULL, builtin_if},
	{"then", NULL, builtin_then},
	{"else", NULL, builtin_else},
	{"fi", NULL, builtin_fi},
#endif
#ifdef CONFIG_HAS_MMU
	{NULL, NULL, NULL}
};

/* Table of forking built-in functions (things that fork cannot change global
 * variables in the parent process, such as the current working directory) */
static struct built_in_command bltins_forking[] = {
#endif
	{"env", "Print all environment variables", builtin_env},
	{"pwd", "Print current directory", builtin_pwd},
	{"help", "List shell built-in commands", builtin_help},
	{NULL, NULL, NULL}
};

#ifndef CONFIG_HAS_MMU
static struct built_in_command bltins_forking[] = {
	{NULL, NULL, NULL}
};
#endif

/* Variables we export */
unsigned int shell_context;  /* Used in cmdedit.c to reset the
								context when someone hits ^C */


/* Globals that are static to this file */
static char *cwd;
static char *local_pending_command = NULL;
static struct jobset job_list = { NULL, NULL };
static int argc;
static char **argv;
static struct close_me *close_me_head;
#ifdef BB_FEATURE_SH_ENVIRONMENT
static int last_bg_pid;
static int last_return_code;
static int show_x_trace;
#endif
#ifdef BB_FEATURE_SH_IF_EXPRESSIONS
static char syntax_err[]="syntax error near unexpected token";
#endif
char *cur_command;

static char *PS1;
static char *PS2;


#ifdef DEBUG_SHELL
static inline void debug_printf(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}
#else
static inline void debug_printf(const char *format, ...) { }
#endif

/*
	Most builtins need access to the struct child_prog that has
	their arguments, previously coded as cmd->progs[0].  That coding
	can exhibit a bug, if the builtin is not the first command in
	a pipeline: "echo foo | exec sort" will attempt to exec foo.

builtin   previous use      notes
------ -----------------  ---------
cd      cmd->progs[0]
env     0
exec    cmd->progs[0]  squashed bug: didn't look for applets or forking builtins
exit    cmd->progs[0]
fg_bg   cmd->progs[0], job_list->head, job_list->fg
help    0
jobs    job_list->head
pwd     0
export  cmd->progs[0]  passes cmd, job_list to builtin_env(), which ignores them
source  cmd->progs[0]
unset   cmd->progs[0]
read    cmd->progs[0]
if      cmd->job_context,  cmd->text
then    cmd->job_context,  cmd->text
else    cmd->job_context,  cmd->text
fi      cmd->job_context

The use of cmd->text by if/then/else/fi is hopelessly hacky.
Would it work to increment cmd->progs[0]->argv and recurse,
somewhat like builtin_exec does?

I added "struct job *family;" to struct child_prog,
and switched API to builtin_foo(struct child_prog *child);
So   cmd->text        becomes  child->family->text
     cmd->job_context  becomes  child->family->job_context
     cmd->progs[0]    becomes  *child
     job_list          becomes  child->family->job_list
 */

static void free_child(struct child_prog *child)
{
	if (!mmu_context_is_original() || !child)
		return;
	free(child->argv[0]);
	free(child->argv);
	free(child);
}

/* built-in 'cd <path>' handler */
static int builtin_cd(struct child_prog *child)
{
	char *newdir;
	int ret = EXIT_SUCCESS;

	if (child->argv[1] == NULL)
		newdir = getenv("HOME");
	else
		newdir = child->argv[1];
	if (chdir(newdir)) {
		fprintf(stderr, "cd: %s: %s\n", newdir, strerror(errno));
		ret = EXIT_FAILURE;
	}
	else
		getcwd(cwd, sizeof(char)*_MAX_LINE);
	free_child(child);

	return ret;
}

/* built-in 'env' handler */
static int builtin_env(struct child_prog *dummy)
{
	char **e;

	for (e = environ; *e; e++) {
		printf( "%s\n", *e);
	}
	free_child(dummy);
	return (0);
}

/* built-in 'exec' handler */
static int builtin_exec(struct child_prog *child)
{
	if (child->argv[1] == NULL)
	{
		free_child(child);
		return EXIT_SUCCESS;   /* Really? */
	}
	child->argv++;
	close_all();
	pseudo_exec(child);
	/* never returns */
}

/* built-in 'exit' handler */
static int builtin_exit(struct child_prog *child)
{
	int ret = EXIT_SUCCESS;
	if (child->argv[1])
		ret = atoi(child->argv[1]);
	free_child(child);
	exit(ret);
}

/* built-in 'fg' and 'bg' handler */
static int builtin_fg_bg(struct child_prog *child)
{
	int i, jobNum, ret = EXIT_FAILURE;
	struct job *job=NULL;
	
	if (!child->argv[1] || child->argv[2]) {
		error_msg("%s: exactly one argument is expected",
				child->argv[0]);
		goto Exit;
	}

	if (sscanf(child->argv[1], "%%%d", &jobNum) != 1) {
		error_msg("%s: bad argument '%s'",
				child->argv[0], child->argv[1]);
		goto Exit;
	}

	for (job = child->family->job_list->head; job; job = job->next) {
		if (job->jobid == jobNum) {
			break;
		}
	}

	if (!job) {
		error_msg("%s: unknown job %d",
				child->argv[0], jobNum);
		goto Exit;
	}

	if (*child->argv[0] == 'f') {
		/* Make this job the foreground job */
		/* suppress messages when run from /linuxrc mag@sysgo.de */
		if (tcsetpgrp(0, job->pgrp) && errno != ENOTTY && tcgetpgrp(0) != -1)
			perror_msg("tcsetpgrp");
		child->family->job_list->fg = job;
	}

	/* Restart the processes in the job */
	for (i = 0; i < job->num_progs; i++)
		job->progs[i].is_stopped = 0;

	kill(-job->pgrp, SIGCONT);

	job->stopped_progs = 0;
	ret = EXIT_SUCCESS;

Exit:
	free_child(child);
	return ret;
}

/* built-in 'help' handler */
static int builtin_help(struct child_prog *dummy)
{
	struct built_in_command *x;

	printf("\nBuilt-in commands:\n");
	printf("-------------------\n");
	for (x = bltins; x->cmd; x++) {
		if (x->descr==NULL)
			continue;
		printf("%s\t%s\n", x->cmd, x->descr);
	}
	for (x = bltins_forking; x->cmd; x++) {
		if (x->descr==NULL)
			continue;
		printf("%s\t%s\n", x->cmd, x->descr);
	}
	printf("\n\n");
	free_child(dummy);
	return EXIT_SUCCESS;
}

/* built-in 'jobs' handler */
static int builtin_jobs(struct child_prog *child)
{
	struct job *job;
	char *status_string;

	for (job = child->family->job_list->head; job; job = job->next) {
		if (job->running_progs == job->stopped_progs)
			status_string = "Stopped";
		else
			status_string = "Running";

		printf(JOB_STATUS_FORMAT, job->jobid, status_string, job->text);
	}
	free_child(child);
	return EXIT_SUCCESS;
}


/* built-in 'pwd' handler */
static int builtin_pwd(struct child_prog *dummy)
{
	getcwd(cwd, _MAX_LINE);
	printf( "%s\n", cwd);
	free_child(dummy);
	return EXIT_SUCCESS;
}

/* built-in 'export VAR=value' handler */
static int builtin_export(struct child_prog *child)
{
	int res;
	char *v = child->argv[1];

	if (v == NULL) {
		return (builtin_env(child));
	}
	res = putenv(v);
	if (res)
		fprintf(stderr, "export: %s\n", strerror(errno));
#ifndef BB_FEATURE_SH_SIMPLE_PROMPT
	if (strncmp(v, "PS1=", 4)==0)
		PS1 = getenv("PS1");
	else if (strncmp(v, "PS2=", 4)==0)
		PS2 = getenv("PS2");
#endif
	free_child(child);
	return (res);
}

/* built-in 'read VAR' handler */
static int builtin_read(struct child_prog *child)
{
	int res = 0, len, newlen;
	char *s;
	char string[MAX_READ];

	if (child->argv[1]) {
		/* argument (VAR) given: put "VAR=" into buffer */
		strcpy(string, child->argv[1]);
		len = strlen(string);
		string[len++] = '=';
		string[len]   = '\0';
		fgets(&string[len], sizeof(string) - len, stdin);	/* read string */
		newlen = strlen(string);
		if(newlen > len)
			string[--newlen] = '\0';	/* chomp trailing newline */
		/*
		** string should now contain "VAR=<value>"
		** copy it (putenv() won't do that, so we must make sure
		** the string resides in a static buffer!)
		*/
		res = -1;
		if((s = strdup(string)))
			res = putenv(s);
		if (res)
			fprintf(stderr, "read: %s\n", strerror(errno));
	}
	else
		fgets(string, sizeof(string), stdin);
	free_child(child);

	return (res);
}

#ifdef BB_FEATURE_SH_IF_EXPRESSIONS
/* Built-in handler for 'if' commands */
static int builtin_if(struct child_prog *child)
{
	struct job *cmd = child->family;
	int status;
	char* charptr1=cmd->text+3; /* skip over the leading 'if ' */

	/* Now run the 'if' command */
	debug_printf( "job=%p entering builtin_if ('%s')-- context=%d\n", cmd, charptr1, cmd->job_context);
	status = run_command_predicate(charptr1);
	debug_printf( "if test returned ");
	if (status == 0) {
		debug_printf( "TRUE\n");
		cmd->job_context |= IF_TRUE_CONTEXT;
	} else {
		debug_printf( "FALSE\n");
		cmd->job_context |= IF_FALSE_CONTEXT;
	}
	debug_printf("job=%p builtin_if set job context to %x\n", cmd, cmd->job_context);
	shell_context++;
	free_child(child);

	return status;
}

/* Built-in handler for 'then' (part of the 'if' command) */
static int builtin_then(struct child_prog *child)
{
	struct job *cmd = child->family;
	char* charptr1=cmd->text+5; /* skip over the leading 'then ' */
	int ret;

	debug_printf( "job=%p entering builtin_then ('%s')-- context=%d\n", cmd, charptr1, cmd->job_context);
	if (! (cmd->job_context & (IF_TRUE_CONTEXT|IF_FALSE_CONTEXT))) {
		shell_context = 0; /* Reset the shell's context on an error */
		error_msg("%s `then'", syntax_err);
		ret = EXIT_FAILURE;
		goto Exit;
	}

	cmd->job_context |= THEN_EXP_CONTEXT;
	debug_printf("job=%p builtin_then set job context to %x\n", cmd, cmd->job_context);

	/* If the if result was FALSE, skip the 'then' stuff */
	if (cmd->job_context & IF_FALSE_CONTEXT) {
		ret = EXIT_SUCCESS;
		goto Exit;
	}

	/* Seems the if result was TRUE, so run the 'then' command */
	debug_printf( "'then' now running '%s'\n", charptr1);
	ret = run_command_predicate(charptr1);

Exit:
	free_child(child);
	return ret;
}

/* Built-in handler for 'else' (part of the 'if' command) */
static int builtin_else(struct child_prog *child)
{
	struct job *cmd = child->family;
	char* charptr1=cmd->text+5; /* skip over the leading 'else ' */
	int ret;

	debug_printf( "job=%p entering builtin_else ('%s')-- context=%d\n", cmd, charptr1, cmd->job_context);

	if (! (cmd->job_context & THEN_EXP_CONTEXT)) {
		shell_context = 0; /* Reset the shell's context on an error */
		error_msg("%s `else'", syntax_err);
		ret = EXIT_FAILURE;
		goto Exit;
	}
	/* If the if result was TRUE, skip the 'else' stuff */
	if (cmd->job_context & IF_TRUE_CONTEXT) {
		ret = EXIT_SUCCESS;
		goto Exit;
	}

	cmd->job_context |= ELSE_EXP_CONTEXT;
	debug_printf("job=%p builtin_else set job context to %x\n", cmd, cmd->job_context);

	/* Now run the 'else' command */
	debug_printf( "'else' now running '%s'\n", charptr1);
	ret = run_command_predicate(charptr1);
Exit:
	free_child(child);
	return ret;
}

/* Built-in handler for 'fi' (part of the 'if' command) */
static int builtin_fi(struct child_prog *child)
{
	struct job *cmd = child->family;
	debug_printf( "job=%p entering builtin_fi ('%s')-- context=%d\n", cmd, "", cmd->job_context);
	if (! (cmd->job_context & (IF_TRUE_CONTEXT|IF_FALSE_CONTEXT))) {
		shell_context = 0; /* Reset the shell's context on an error */
		error_msg("%s `fi'", syntax_err);
		free_child(child);
		return EXIT_FAILURE;
	}
	/* Clear out the if and then context bits */
	cmd->job_context &= ~(IF_TRUE_CONTEXT|IF_FALSE_CONTEXT|THEN_EXP_CONTEXT|ELSE_EXP_CONTEXT);
	debug_printf("job=%p builtin_fi set job context to %x\n", cmd, cmd->job_context);
	shell_context--;
	free_child(child);
	return EXIT_SUCCESS;
}
#endif

/* Built-in '.' handler (read-in and execute commands from file) */
static int builtin_source(struct child_prog *child)
{
	FILE *input;
	int status = EXIT_FAILURE;
	int fd;

	if (child->argv[1] == NULL)
		goto Exit;

	input = fopen(child->argv[1], "r");
	if (!input) {
		printf( "Couldn't open file '%s'\n", child->argv[1]);
		goto Exit;
	}

	fd=fileno(input);
	mark_open(fd);
	/* Now run the file */
	status = busy_loop(input);
	fclose(input);
	mark_closed(fd);

Exit:
	free_child(child);
	return (status);
}

/* built-in 'unset VAR' handler */
static int builtin_unset(struct child_prog *child)
{
	if (child->argv[1] == NULL) {
		printf( "unset: parameter required.\n");
		free_child(child);
		return EXIT_FAILURE;
	}
	unsetenv(child->argv[1]);
	free_child(child);
	return EXIT_SUCCESS;
}

#ifdef BB_FEATURE_SH_IF_EXPRESSIONS
/* currently used by if/then/else.
 *
 * Reparsing the command line for this purpose is gross,
 * incorrect, and fundamentally unfixable; in particular,
 * think about what happens with command substitution.
 * We really need to pull out the run, wait, return status
 * functionality out of busy_loop so we can child->argv++
 * and use that, without going back through parse_command.
 */
static int run_command_predicate(char *cmd)
{
	int n=strlen(cmd);
	local_pending_command = xmalloc(n+1);
	strncpy(local_pending_command, cmd, n); 
	local_pending_command[n]='\0';
	return( busy_loop(NULL));
}
#endif

static void mark_open(int fd)
{
	struct close_me *new = xmalloc(sizeof(struct close_me));
	new->fd = fd;
	new->next = close_me_head;
	close_me_head = new;
}

static void mark_closed(int fd)
{
	struct close_me *tmp;
	if (close_me_head == NULL || close_me_head->fd != fd)
		error_msg_and_die("corrupt close_me");
	tmp = close_me_head;
	close_me_head = close_me_head->next;
	free(tmp);
}

static void close_all_no_free(void)
{
	struct close_me *c;
	for (c=close_me_head; c; c=c->next) 
		close(c->fd);
}

static void close_all()
{
	struct close_me *c, *tmp;
	for (c=close_me_head; c; c=tmp) {
		close(c->fd);
		tmp=c->next;
		free(c);
	}
	close_me_head = NULL;
}


/* free up all memory from a job */
static void free_job(struct job *cmd)
{
	int i;
	struct jobset *keep;

	for (i = 0; i < cmd->num_progs; i++) {
		free(cmd->progs[i].argv);
		if (cmd->progs[i].redirects)
			free(cmd->progs[i].redirects);
	}
	if (cmd->progs)
		free(cmd->progs);
	if (cmd->text)
		free(cmd->text);
	if (cmd->cmdbuf)
		free(cmd->cmdbuf);
	keep = cmd->job_list;
	memset(cmd, 0, sizeof(struct job));
	cmd->job_list = keep;
}

/* remove a job from the job_list */
static void remove_job(struct jobset *job_list, struct job *job)
{
	struct job *prevjob;

	free_job(job);
	if (job == job_list->head) {
		job_list->head = job->next;
	} else {
		prevjob = job_list->head;
		while (prevjob->next != job)
			prevjob = prevjob->next;
		prevjob->next = job->next;
	}

	free(job);
}

/* Checks to see if any background processes have exited -- if they 
   have, figure out why and see if a job has completed */
static void checkjobs(struct jobset *job_list)
{
	struct job *job;
	pid_t childpid;
	int status;
	int prognum = 0;

	while ((childpid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
		for (job = job_list->head; job; job = job->next) {
			prognum = 0;
			while (prognum < job->num_progs &&
				   job->progs[prognum].pid != childpid) prognum++;
			if (prognum < job->num_progs)
				break;
		}

		/* This happens on backticked commands */
		if(job==NULL)
			return;

		if (WIFEXITED(status) || WIFSIGNALED(status)) {
			/* child exited */
			if(WIFSIGNALED(status))
				printf("[%d] %s\n",job->progs[prognum].pid,
						(char *)strsignal (WTERMSIG(status)));
			job->running_progs--;
			job->progs[prognum].pid = 0;

			if (!job->running_progs) {
				printf(JOB_STATUS_FORMAT, job->jobid, "Done", job->text);
				remove_job(job_list, job);
			}
		} else {
			/* child stopped */
			job->stopped_progs++;
			job->progs[prognum].is_stopped = 1;

			if (job->stopped_progs == job->num_progs) {
				printf(JOB_STATUS_FORMAT, job->jobid, "Stopped",
					   job->text);
			}
		}
	}

	if (childpid == -1 && errno != ECHILD)
		perror_msg("waitpid");
}

/* squirrel != NULL means we squirrel away copies of stdin, stdout,
 * and stderr if they are redirected. */
static int setup_redirects(struct child_prog *prog, int squirrel[])
{
	int i;
	int openfd;
	int mode = O_RDONLY;
	struct redir_struct *redir = prog->redirects;

	for (i = 0; i < prog->num_redirects; i++, redir++) {
		switch (redir->type) {
		case REDIRECT_INPUT:
			mode = O_RDONLY;
			break;
		case REDIRECT_OVERWRITE:
			mode = O_WRONLY | O_CREAT | O_TRUNC;
			break;
		case REDIRECT_APPEND:
			mode = O_WRONLY | O_CREAT | O_APPEND;
			break;
		}

		openfd = open(redir->filename, mode, 0666);
		if (openfd < 0) {
			/* this could get lost if stderr has been redirected, but
			   bash and ash both lose it as well (though zsh doesn't!) */
			perror_msg("error opening %s", redir->filename);
			return 1;
		}

		if (openfd != redir->fd) {
			if (squirrel && redir->fd < 3) {
				squirrel[redir->fd] = dup(redir->fd);
			}
			dup2(openfd, redir->fd);
			close(openfd);
		}
	}

	return 0;
}

static void restore_redirects(int squirrel[])
{
	int i, fd;
	for (i=0; i<3; i++) {
		fd = squirrel[i];
		if (fd != -1) {
			/* No error checking.  I sure wouldn't know what
			 * to do with an error if I found one! */
			dup2(fd, i);
			close(fd);
		}
	}
}

static inline void cmdedit_set_initial_prompt(void)
{
#ifdef BB_FEATURE_SH_SIMPLE_PROMPT
	PS1 = NULL;
	PS2 = "> ";
#else
	PS1 = getenv("PS1");
	if(PS1==0) {
		PS1 = "\\w \\$ ";
	}
	PS2 = getenv("PS2");
	if(PS2==0) 
		PS2 = "> ";
#endif	
}

static inline void setup_prompt_string(char **prompt_str)
{
#ifdef BB_FEATURE_SH_SIMPLE_PROMPT
	/* Set up the prompt */
	if (shell_context == 0) {
		if (PS1)
			free(PS1);
		PS1=xmalloc(strlen(cwd)+4);
		sprintf(PS1, "%s %s", cwd, ( geteuid() != 0 ) ?  "$ ":"# ");
		*prompt_str = PS1;
	} else {
		*prompt_str = PS2;
	}
#else
	*prompt_str = (shell_context==0)? PS1 : PS2;
#endif	
}

static int get_command(FILE * source, char *command)
{
	char *prompt_str;

	if (source == NULL) {
		if (local_pending_command) {
			/* a command specified (-c option): return it & mark it done */
			strcpy(command, local_pending_command);
			free(local_pending_command);
			local_pending_command = NULL;
			return 0;
		}
		return 1;
	}

	if (source == stdin) {
		setup_prompt_string(&prompt_str);

#ifdef BB_FEATURE_COMMAND_EDITING
		/*
		** enable command line editing only while a command line
		** is actually being read; otherwise, we'll end up bequeathing
		** atexit() handlers and other unwanted stuff to our
		** child processes (rob@sysgo.de)
		*/
		cmdedit_read_input(prompt_str, command);
		cmdedit_terminate();
		return 0;
#else
		fputs(prompt_str, stdout);
#endif
	}

	if (!fgets(command, BUFSIZ - 2, source)) {
		if (source == stdin)
			printf("\n");
		return 1;
	}

	/* remove trailing newline */
	chomp(command);

	return 0;
}

#ifdef BB_FEATURE_SH_ENVIRONMENT
static char* itoa(register int i)
{
	static char a[7]; /* Max 7 ints */
	register char *b = a + sizeof(a) - 1;
	int   sign = (i < 0);

	if (sign)
		i = -i;
	*b = 0;
	do
	{
		*--b = '0' + (i % 10);
		i /= 10;
	}
	while (i);
	if (sign)
		*--b = '-';
	return b;
}
#endif	

#if defined BB_FEATURE_SH_ENVIRONMENT && ! defined BB_FEATURE_SH_WORDEXP
char * strsep_space( char *string, int * index)
{
	char *token, *begin;

	begin = string;

	/* Short circuit the trivial case */
	if ( !string || ! string[*index])
		return NULL;

	/* Find the end of the token. */
	while( string && string[*index] && !isspace(string[*index]) ) {
		(*index)++;
	}

	/* Find the end of any whitespace trailing behind 
	 * the token and let that be part of the token */
	while( string && string[*index] && isspace(string[*index]) ) {
		(*index)++;
	}

	if (! string && *index==0) {
		/* Nothing useful was found */
		return NULL;
	}

	token = xmalloc(*index+1);
	token[*index] = '\0';
	strncpy(token, string,  *index); 

	return token;
}
#endif	


static int expand_arguments(char *command)
{
	int index = 0;
#ifdef BB_FEATURE_SH_ENVIRONMENT
	expand_t expand_result;
	char *src, *dst, *var;
	int i=0, length, total_length=0, retval;
	const char *out_of_space = "out of space during expansion"; 
#endif

	/* get rid of the terminating \n */
	chomp(command);
	
	/* Fix up escape sequences to be the Real Thing(tm) */
	while( command && command[index]) {
		if (command[index] == '\\') {
			char *tmp = command+index+1;
			command[index] = process_escape_sequence(  &tmp );
			memmove(command+index+1, tmp, strlen(tmp)+1);
		}
		index++;
	}

#ifdef BB_FEATURE_SH_ENVIRONMENT


#if BB_FEATURE_SH_WORDEXP
	/* This first part uses wordexp() which is a wonderful C lib 
	 * function which expands nearly everything.  */ 
	retval = wordexp (command, &expand_result, WRDE_SHOWERR);
	if (retval == WRDE_NOSPACE) {
		/* Mem may have been allocated... */
		wordfree (&expand_result);
		error_msg(out_of_space);
		return FALSE;
	}
	if (retval < 0) {
		/* Some other error.  */
		error_msg("syntax error");
		return FALSE;
	}
	
	if (expand_result.we_wordc > 0) {
		/* Convert from char** (one word per string) to a simple char*,
		 * but don't overflow command which is BUFSIZ in length */
		*command = '\0';
		while (i < expand_result.we_wordc && total_length < BUFSIZ) {
			length=strlen(expand_result.we_wordv[i])+1;
			if (BUFSIZ-total_length-length <= 0) {
				error_msg(out_of_space);
				return FALSE;
			}
			strcat(command+total_length, expand_result.we_wordv[i++]);
			strcat(command+total_length, " ");
			total_length+=length;
		}
		wordfree (&expand_result);
	}
#else

	/* Ok.  They don't have a recent glibc and they don't have uClibc.  Chances
	 * are about 100% they don't have wordexp(). So instead the best we can do
	 * is use glob and then fixup environment variables and such ourselves.
	 * This is better then nothing, but certainly not perfect */

	/* It turns out that glob is very stupid.  We have to feed it one word at a
	 * time since it can't cope with a full string.  Here we convert command
	 * (char*) into cmd (char**, one word per string) */
	{
        
		int flags = GLOB_NOCHECK
#ifdef GLOB_BRACE
				| GLOB_BRACE
#endif	
#ifdef GLOB_TILDE
				| GLOB_TILDE
#endif	
			;
		char *tmpcmd, *cmd, *cmd_copy;
		/* We need a clean copy, so strsep can mess up the copy while
		 * we write stuff into the original (in a minute) */
		cmd = cmd_copy = xstrdup(command);
		*command = '\0';
		for (index = 0, tmpcmd = cmd; 
				(tmpcmd = strsep_space(cmd, &index)) != NULL; cmd += index, index=0) {
			if (*tmpcmd == '\0')
				break;
			retval = glob(tmpcmd, flags, NULL, &expand_result);
			free(tmpcmd); /* Free mem allocated by strsep_space */
			if (retval == GLOB_NOSPACE) {
				/* Mem may have been allocated... */
				globfree (&expand_result);
				error_msg(out_of_space);
				return FALSE;
			} else if (retval != 0) {
				/* Some other error.  GLOB_NOMATCH shouldn't
				 * happen because of the GLOB_NOCHECK flag in 
				 * the glob call. */
				error_msg("syntax error");
				return FALSE;
			} else {
			/* Convert from char** (one word per string) to a simple char*,
			 * but don't overflow command which is BUFSIZ in length */
				for (i=0; i < expand_result.gl_pathc; i++) {
					length=strlen(expand_result.gl_pathv[i]);
					if (total_length+length+1 >= BUFSIZ) {
						error_msg(out_of_space);
						return FALSE;
					}
					if (i>0) {
						strcat(command+total_length, " ");
						total_length+=1;
					}
					strcat(command+total_length, expand_result.gl_pathv[i]);
					total_length+=length;
				}
				globfree (&expand_result);
			}
		}
		free(cmd_copy);
		trim(command);
	}
	
#endif	

	/* Now do the shell variable substitutions which 
	 * wordexp can't do for us, namely $? and $! */
	src = command;
	while((dst = strchr(src,'$')) != NULL){
		var = NULL;
		switch(*(dst+1)) {
			case '?':
				var = itoa(last_return_code);
				break;
			case '!':
				if (last_bg_pid==-1)
					*(var)='\0';
				else
					var = itoa(last_bg_pid);
				break;
				/* Everything else like $$, $#, $[0-9], etc should all be
				 * expanded by wordexp(), so we can in theory skip that stuff
				 * here, but just to be on the safe side (i.e. since uClibc
				 * wordexp doesn't do this stuff yet), lets leave it in for
				 * now. */
			case '$':
				var = itoa(getpid());
				break;
			case '#':
				var = itoa(argc-1);
				break;
			case '0':case '1':case '2':case '3':case '4':
			case '5':case '6':case '7':case '8':case '9':
				{
					int index=*(dst + 1)-48;
					if (index >= argc) {
						var='\0';
					} else {
						var = argv[index];
					}
				}
				break;

		}
		if (var) {
			/* a single character construction was found, and 
			 * already handled in the case statement */
			src=dst+2;
		} else {
			/* Looks like an environment variable */
			char delim_hold;
			int num_skip_chars=1;
			int dstlen = strlen(dst);
			/* Is this a ${foo} type variable? */
			if (dstlen >=2 && *(dst+1) == '{') {
				src=strchr(dst+1, '}');
				num_skip_chars=2;
			} else {
				src=strpbrk(dst+1, " \t~`!$^&*()=|\\[];\"'<>?./");
			}
			if (src == NULL) {
				src = dst+dstlen;
			}
			delim_hold=*src;
			*src='\0';  /* temporary */
			var = getenv(dst + num_skip_chars);
			*src=delim_hold;
			if (num_skip_chars==2) {
				src++;
			}
		}
		if (var == NULL) {
			/* Seems we got an un-expandable variable.  So delete it. */
			var = "";
		}
		{
			int subst_len = strlen(var);
			int trail_len = strlen(src);
			if (dst+subst_len+trail_len >= command+BUFSIZ) {
				error_msg(out_of_space);
				return FALSE;
			}
			/* Move stuff to the end of the string to accommodate
			 * filling the created gap with the new stuff */
			memmove(dst+subst_len, src, trail_len);
			*(dst+subst_len+trail_len)='\0';
			/* Now copy in the new stuff */
			memcpy(dst, var, subst_len);
			src = dst+subst_len;
		}
	}

#endif	
	return TRUE;
}

/* Return cmd->num_progs as 0 if no command is present (e.g. an empty
   line). If a valid command is found, command_ptr is set to point to
   the beginning of the next command (if the original command had more 
   then one job associated with it) or NULL if no more commands are 
   present. */
static int parse_command(char **command_ptr, struct job *job, int *inbg)
{
	char *command;
	char *return_command = NULL;
	char *src, *buf, *chptr;
	int argc_l = 0;
	int done = 0;
	int argv_alloced;
	int i;
	char quote = '\0';
	int count;
	struct child_prog *prog;

	/* skip leading white space */
	while (**command_ptr && isspace(**command_ptr))
		(*command_ptr)++;

	/* this handles empty lines or leading '#' characters */
	if (!**command_ptr || (**command_ptr == '#')) {
		job->num_progs=0;
		return 0;
	}

	*inbg = 0;
	job->num_progs = 1;
	job->progs = xmalloc(sizeof(*job->progs));

	/* We set the argv elements to point inside of this string. The 
	   memory is freed by free_job(). Allocate twice the original
	   length in case we need to quote every single character.

	   Getting clean memory relieves us of the task of NULL 
	   terminating things and makes the rest of this look a bit 
	   cleaner (though it is, admittedly, a tad less efficient) */
	job->cmdbuf = command = xcalloc(2*strlen(*command_ptr) + 1, sizeof(char));
	job->text = NULL;

	prog = job->progs;
	prog->num_redirects = 0;
	prog->redirects = NULL;
	prog->is_stopped = 0;
	prog->family = job;

	argv_alloced = 5;
	prog->argv = xmalloc(sizeof(*prog->argv) * argv_alloced);
	prog->argv[0] = job->cmdbuf;

	buf = command;
	src = *command_ptr;
	while (*src && !done) {
		if (quote == *src) {
			quote = '\0';
		} else if (quote) {
			*buf++ = *src;
		} else if (isspace(*src)) {
			if (*prog->argv[argc_l]) {
				buf++, argc_l++;
				/* +1 here leaves room for the NULL which ends argv */
				if ((argc_l + 1) == argv_alloced) {
					argv_alloced += 5;
					prog->argv = xrealloc(prog->argv,
										  sizeof(*prog->argv) *
										  argv_alloced);
				}
				prog->argv[argc_l] = buf;
			}
		} else
			switch (*src) {
			case '"':
			case '\'':
				quote = *src;
				break;

			case '#':			/* comment */
				if (*(src-1)== '$')
					*buf++ = *src;
				else
					done = 1;
				break;

			case '>':			/* redirects */
			case '<':
				i = prog->num_redirects++;
				prog->redirects = xrealloc(prog->redirects,
											  sizeof(*prog->redirects) *
											  (i + 1));

				prog->redirects[i].fd = -1;
				if (buf != prog->argv[argc_l]) {
					/* the stuff before this character may be the file number 
					   being redirected */
					prog->redirects[i].fd =
						strtol(prog->argv[argc_l], &chptr, 10);

					if (*chptr && *prog->argv[argc_l]) {
						buf++, argc_l++;
						prog->argv[argc_l] = buf;
					}
				}

				if (prog->redirects[i].fd == -1) {
					if (*src == '>')
						prog->redirects[i].fd = 1;
					else
						prog->redirects[i].fd = 0;
				}

				if (*src++ == '>') {
					if (*src == '>')
						prog->redirects[i].type =
							REDIRECT_APPEND, src++;
					else
						prog->redirects[i].type = REDIRECT_OVERWRITE;
				} else {
					prog->redirects[i].type = REDIRECT_INPUT;
				}

				/* This isn't POSIX sh compliant. Oh well. */
				chptr = src;
				while (isspace(*chptr))
					chptr++;

				if (!*chptr) {
					error_msg("file name expected after %c", *(src-1));
					free_job(job);
					job->num_progs=0;
					return 1;
				}

				prog->redirects[i].filename = buf;
				while (*chptr && !isspace(*chptr))
					*buf++ = *chptr++;

				src = chptr - 1;	/* we src++ later */
				prog->argv[argc_l] = ++buf;
				break;

			case '|':			/* pipe */
				/* finish this command */
				if (*prog->argv[argc_l])
					argc_l++;
				if (!argc_l) {
					error_msg("empty command in pipe");
					free_job(job);
					job->num_progs=0;
					return 1;
				}
				prog->argv[argc_l] = NULL;

				/* and start the next */
				job->num_progs++;
				job->progs = xrealloc(job->progs,
									  sizeof(*job->progs) * job->num_progs);
				prog = job->progs + (job->num_progs - 1);
				prog->num_redirects = 0;
				prog->redirects = NULL;
				prog->is_stopped = 0;
				prog->family = job;
				argc_l = 0;

				argv_alloced = 5;
				prog->argv = xmalloc(sizeof(*prog->argv) * argv_alloced);
				prog->argv[0] = ++buf;

				src++;
				while (*src && isspace(*src))
					src++;

				if (!*src) {
					error_msg("empty command in pipe");
					free_job(job);
					job->num_progs=0;
					return 1;
				}
				src--;			/* we'll ++ it at the end of the loop */

				break;

			case '&':			/* background */
				*inbg = 1;
			case ';':			/* multiple commands */
				done = 1;
				return_command = *command_ptr + (src - *command_ptr) + 1;
				break;

#ifdef BB_FEATURE_SH_BACKTICKS
			case '`':
				/* Exec a backtick-ed command */
				/* Besides any previous brokenness, I have not
				 * updated backtick handling for close_me support.
				 * I don't know if it needs it or not.  -- LRD */
				{
					char* charptr1=NULL, *charptr2;
					char* ptr=NULL;
					struct job *newjob;
					struct jobset njob_list = { NULL, NULL };
					int pipefd[2];
					int size;

					ptr=strchr(++src, '`');
					if (ptr==NULL) {
						fprintf(stderr, "Unmatched '`' in command\n");
						free_job(job);
						return 1;
					}

					/* Make some space to hold just the backticked command */
					charptr1 = charptr2 = xmalloc(1+ptr-src);
					memcpy(charptr1, src, ptr-src);
					charptr1[ptr-src] = '\0';
					newjob = xmalloc(sizeof(struct job));
					newjob->job_list = &njob_list;
					/* Now parse and run the backticked command */
					if (!parse_command(&charptr1, newjob, inbg) 
							&& newjob->num_progs) {
						pipe(pipefd);
						run_command(newjob, 0, pipefd);
					}
					checkjobs(job->job_list);
					free_job(newjob);  /* doesn't actually free newjob,
					                     looks like a memory leak */
					free(charptr2);
					
					/* Make a copy of any stuff left over in the command 
					 * line after the second backtick */
					charptr2 = xmalloc(strlen(ptr)+1);
					memcpy(charptr2, ptr+1, strlen(ptr));


					/* Copy the output from the backtick-ed command into the
					 * command line, making extra room as needed  */
					--src;
					charptr1 = xmalloc(BUFSIZ);
					while ( (size=full_read(pipefd[0], charptr1, BUFSIZ-1)) >0) {
						int newsize=src - *command_ptr + size + 1 + strlen(charptr2);
						if (newsize > BUFSIZ) {
							*command_ptr=xrealloc(*command_ptr, newsize);
						}
						memcpy(src, charptr1, size); 
						src+=size;
					}
					free(charptr1);
					close(pipefd[0]);
					if (*(src-1)=='\n')
						--src;

					/* Now paste into the *command_ptr all the stuff 
					 * leftover after the second backtick */
					memcpy(src, charptr2, strlen(charptr2)+1);
					free(charptr2);

					/* Now recursively call parse_command to deal with the new
					 * and improved version of the command line with the backtick
					 * results expanded in place... */
					{
						struct jobset *jl=job->job_list;
						free_job(job);
						job->job_list = jl;
					}
					return(parse_command(command_ptr, job, inbg));
				}
				break;
#endif // BB_FEATURE_SH_BACKTICKS

			case '\\':
#ifdef HANDLE_CONTINUATION_CHARS
				src++;
				if (!*src) {
/* This is currently a little broken... */
					/* They fed us a continuation char, so continue reading stuff
					 * on the next line, then tack that onto the end of the current
					 * command */
					char *command;
					int newsize;
					printf("erik: found a continue char at EOL...\n");
					command = (char *) xcalloc(BUFSIZ, sizeof(char));
					if (get_command(input, command)) {
						error_msg("character expected after \\");
						free(command);
						free_job(job);
						return 1;
					}
					newsize = strlen(*command_ptr) + strlen(command) + 2;
					if (newsize > BUFSIZ) {
						printf("erik: doing realloc\n");
						*command_ptr=xrealloc(*command_ptr, newsize);
					}
					printf("erik: A: *command_ptr='%s'\n", *command_ptr);
					memcpy(--src, command, strlen(command)); 
					printf("erik: B: *command_ptr='%s'\n", *command_ptr);
					free(command);
					break;
				}
				if (*src == '*' || *src == '[' || *src == ']'
					|| *src == '?') *buf++ = '\\';
#else
				if (!*(src+1)) 
				{
					error_msg("character expected after \\");
					free_job(job);
					return 1;
				}
#endif
				/* fallthrough */
			default:
				*buf++ = *src;
			}

		src++;
	}

	if (quote)
	{
		error_msg("missing end quote %c", quote);
		free_job(job);
		return 1;
	}

	if (*prog->argv[argc_l]) {
		argc_l++;
	}
	if (!argc_l) {
		free_job(job);
		return 0;
	}
	prog->argv[argc_l] = NULL;

	if (!return_command) {
		job->text = xmalloc(strlen(*command_ptr) + 1);
		strcpy(job->text, *command_ptr);
	} else {
		/* This leaves any trailing spaces, which is a bit sloppy */
		count = return_command - *command_ptr;
		job->text = xmalloc(count + 1);
		strncpy(job->text, *command_ptr, count);
		job->text[count] = '\0';
	}

	*command_ptr = return_command;
	
	return 0;
}

/* Run the child_prog, no matter what kind of command it uses.
 */
static int pseudo_exec(struct child_prog *child)
{
	struct built_in_command *x;
#ifdef BB_FEATURE_SH_STANDALONE_SHELL
	char *name;
#endif

	/* Check if the command matches any of the non-forking builtins.
	 * Depending on context, this might be redundant.  But it's
	 * easier to waste a few CPU cycles than it is to figure out
	 * if this is one of those cases.
	 */
	for (x = bltins; x->cmd; x++) {
		if (strcmp(child->argv[0], x->cmd) == 0 ) {
			exit(x->function(child));
		}
	}

	/* Check if the command matches any of the forking builtins. */
	for (x = bltins_forking; x->cmd; x++) {
		if (strcmp(child->argv[0], x->cmd) == 0) {
			applet_name=x->cmd;
			exit (x->function(child));
		}
	}
#ifdef BB_FEATURE_SH_STANDALONE_SHELL
	/* Check if the command matches any busybox internal
	 * commands ("applets") here.  Following discussions from
	 * November 2000 on busybox@opensource.lineo.com, don't use
	 * get_last_path_component().  This way explicit (with
	 * slashes) filenames will never be interpreted as an
	 * applet, just like with builtins.  This way the user can
	 * override an applet with an explicit filename reference.
	 * The only downside to this change is that an explicit
	 * /bin/foo invocation will fork and exec /bin/foo, even if
	 * /bin/foo is a symlink to busybox.
	 */
	name = child->argv[0];

#ifdef BB_FEATURE_SH_APPLETS_ALWAYS_WIN
	/* If you enable BB_FEATURE_SH_APPLETS_ALWAYS_WIN, then
	 * if you run /bin/cat, it will use BusyBox cat even if 
	 * /bin/cat exists on the filesystem and is _not_ busybox.
	 * Some systems want this, others do not.  Choose wisely.  :-)
	 */
	name = get_last_path_component(name);
#endif

	{
	    char** argv=child->argv;
	    int argc_l;
	    for(argc_l=0;*argv!=NULL; argv++, argc_l++);
	    optind = 1;
	    run_applet_by_name(name, argc_l, child->argv);
	}
#endif

	execvp(child->argv[0], child->argv);
	perror_msg_and_die("%s", child->argv[0]);
}

static void insert_job(struct job *newjob, int inbg)
{
	struct job *thejob;
	struct jobset *job_list=newjob->job_list;

	/* find the ID for thejob to use */
	newjob->jobid = 1;
	for (thejob = job_list->head; thejob; thejob = thejob->next)
		if (thejob->jobid >= newjob->jobid)
			newjob->jobid = thejob->jobid + 1;

	/* add thejob to the list of running jobs */
	if (!job_list->head) {
		thejob = job_list->head = xmalloc(sizeof(*thejob));
	} else {
		for (thejob = job_list->head; thejob->next; thejob = thejob->next) /* nothing */;
		thejob->next = xmalloc(sizeof(*thejob));
		thejob = thejob->next;
	}

	*thejob = *newjob;   /* physically copy the struct job */
	thejob->next = NULL;
	thejob->running_progs = thejob->num_progs;
	thejob->stopped_progs = 0;

	if (inbg) {
		/* we don't wait for background thejobs to return -- append it 
		   to the list of backgrounded thejobs and leave it alone */
		printf("[%d] %d\n", thejob->jobid,
			   newjob->progs[newjob->num_progs - 1].pid);
#ifdef BB_FEATURE_SH_ENVIRONMENT
		last_bg_pid=newjob->progs[newjob->num_progs - 1].pid;
#endif
	} else {
		newjob->job_list->fg = thejob;

		/* move the new process group into the foreground */
		/* suppress messages when run from /linuxrc mag@sysgo.de */
		if (tcsetpgrp(0, newjob->pgrp) && errno != ENOTTY && tcgetpgrp(0) != -1)
			perror_msg("tcsetpgrp");
		// tell child to continue if got SIGTTIN
		kill(newjob->pgrp, SIGCONT);
	}
}

static int run_command(struct job *newjob, int inbg, int outpipe[2])
{
	/* struct job *thejob; */
	int i, leader;
	int nextin, nextout;
	int pipefds[2];				/* pipefd[0] is for reading */
	struct built_in_command *x;
	struct child_prog *child;

	nextin = 0, nextout = 1;
	for (i = 0; i < newjob->num_progs; i++) {
		child = & (newjob->progs[i]);

		if ((i + 1) < newjob->num_progs) {
			if (pipe(pipefds)<0) perror_msg_and_die("pipe");
			nextout = pipefds[1];
		} else {
			if (outpipe[1]!=-1) {
				nextout = outpipe[1];
			} else {
				nextout = 1;
			}
		}

#ifdef BB_FEATURE_SH_ENVIRONMENT
		if (show_x_trace==TRUE) {
			int j;
			fputc('+', stderr);
			for (j = 0; child->argv[j]; j++) {
				fputc(' ', stderr);
				fputs(child->argv[j], stderr);
			}
			fputc('\n', stderr);
		}
#endif

		/* Check if the command matches any non-forking builtins,
		 * but only if this is a simple command.
		 * Non-forking builtins within pipes have to fork anyway,
		 * and are handled in pseudo_exec.  "echo foo | read bar"
		 * is doomed to failure, and doesn't work on bash, either.
		 */
		if (newjob->num_progs == 1) {
			for (x = bltins; x->cmd; x++) {
				if (strcmp(child->argv[0], x->cmd) == 0 ) {
					int squirrel[] = {-1, -1, -1};
					int rcode;
					free(newjob->text);
					setup_redirects(child, squirrel);
					rcode = x->function(child);
					restore_redirects(squirrel);
					return rcode;
				}
			}
		}

		if(!(child->pid = fork()))
		{
			/* put the process into first ID's group */ 

			leader = i ? newjob->progs[0].pid : 0;
			if(setpgid(0, leader))
			{
				perror("cannot setpgid()");
				if (mmu_context_is_original())
				{
					free(newjob->text);
					free_child(child);
				}
				exit(1);
			}

			signal(SIGTTOU, SIG_DFL);
			if (mmu_context_is_original())
				close_all();
			else
				close_all_no_free();
			
			if (outpipe[1]!=-1) {
				close(outpipe[0]);
			}
			if (nextin != 0) {
				dup2(nextin, 0);
				close(nextin);
			}

			if (nextout != 1) {
				dup2(nextout, 1);
				dup2(nextout, 2);  /* Really? */
				close(nextout);
				close(pipefds[0]);
			}

			/* explicit redirects override pipes */
			setup_redirects(child,NULL);
		
			if (mmu_context_is_original())
				free(newjob->text);
			pseudo_exec(child);
		}

		if (child->pid < 0 )   /* fork failed */
		{
			free(newjob->text);
			perror_msg("cannot fork, program not executed");
			return 1;           /* return value is not checked */
		}

		if (outpipe[1]!=-1) {
			close(outpipe[1]);
		}

		/* put our child in the process group whose leader is the
		   first process in this pipe */
		setpgid(child->pid, newjob->progs[0].pid);
		if (nextin != 0)
			close(nextin);
		if (nextout != 1)
			close(nextout);

		/* If there isn't another process, nextin is garbage 
		   but it doesn't matter */
		nextin = pipefds[0];
	}

	newjob->pgrp = newjob->progs[0].pid;

	insert_job(newjob, inbg);

	return 0;
}

static int busy_loop(FILE * input)
{
	char *next_command = NULL;
	struct job newjob;
	pid_t  parent_pgrp;
	int i;
	int inbg;
	int status;
	newjob.job_list = &job_list;
	newjob.job_context = DEFAULT_CONTEXT;

	/* save current owner of TTY so we can restore it on exit */
	parent_pgrp = tcgetpgrp(0);

	cur_command = (char *) xcalloc(BUFSIZ, sizeof(char));

	/* don't pay any attention to this signal; it just confuses 
	   things and isn't really meant for shells anyway */
	signal(SIGTTOU, SIG_IGN);

	while (1) {
		if (!job_list.fg) {
			/* no job is in the foreground */

			/* see if any background processes have exited */
			checkjobs(&job_list);

			if (!next_command) {
				if (get_command(input, cur_command))
					break;
				next_command = cur_command;
			}

			if (expand_arguments(next_command) == FALSE) {
				cur_command = (char *) xrealloc(cur_command, BUFSIZ*sizeof(char));
				*cur_command = 0;
				next_command = NULL;
				continue;
			}

			if (!parse_command(&next_command, &newjob, &inbg) &&
				newjob.num_progs) {
				int pipefds[2] = {-1,-1};
				debug_printf( "job=%p fed to run_command by busy_loop()'\n", 
						&newjob);
				run_command(&newjob, inbg, pipefds);
			}
			else {
				cur_command = (char *) xrealloc(cur_command, BUFSIZ*sizeof(char));
				*cur_command = 0;
				next_command = NULL;
			}
		} else {
			/* a job is running in the foreground; wait for it */
			i = 0;
			while ( (i < job_list.fg->num_progs) && (!job_list.fg->progs[i].pid ||
				job_list.fg->progs[i].is_stopped == 1)) 
				i++;

			if (waitpid(job_list.fg->progs[i].pid, &status, WUNTRACED)<0)
				perror_msg_and_die("waitpid(%d)",job_list.fg->progs[i].pid);

			if (WIFEXITED(status) || WIFSIGNALED(status)) {
				if(WIFSIGNALED(status))
					printf("[%d] %s\n",job_list.fg->progs[i].pid,
					    (char *)strsignal (WTERMSIG(status)));
				
				/* the child exited */
				job_list.fg->running_progs--;
				job_list.fg->progs[i].pid = 0;

#ifdef BB_FEATURE_SH_ENVIRONMENT
				last_return_code=WEXITSTATUS(status);
				debug_printf("'%s' exited -- return code %d\n",
						job_list.fg->text, last_return_code);
#endif
				if (!job_list.fg->running_progs) {
					/* child exited */
					remove_job(&job_list, job_list.fg);
					job_list.fg = NULL;
				}
			} else {
				/* the child was stopped */
				job_list.fg->stopped_progs++;
				job_list.fg->progs[i].is_stopped = 1;

				if (job_list.fg->stopped_progs == job_list.fg->running_progs) {
					printf("\n" JOB_STATUS_FORMAT, job_list.fg->jobid,
						   "Stopped", job_list.fg->text);
					job_list.fg = NULL;
				}
			}

			if (!job_list.fg) {
				/* move the shell to the foreground */
				/* suppress messages when run from /linuxrc mag@sysgo.de */
				if (tcsetpgrp(0, getpgrp()) && errno != ENOTTY &&
					tcgetpgrp(0) != -1)
					perror_msg("tcsetpgrp"); 
			}
		}
	}

	/* return controlling TTY back to parent process group before exiting */
	if (tcsetpgrp(0, parent_pgrp) && errno != ENOTTY && tcgetpgrp(0) != -1)
		perror_msg("tcsetpgrp");

	/* return exit status if called with "-c" */
	if (input == NULL && WIFEXITED(status))
		return WEXITSTATUS(status);
	
	return 0;
}


#ifdef BB_FEATURE_CLEAN_UP
void free_memory(void)
{
	free(cur_command);
	if (cwd)
		free(cwd);
	if (local_pending_command)
		free(local_pending_command);

	if (job_list.fg && !job_list.fg->running_progs) {
		remove_job(&job_list, job_list.fg);
	}
}
#endif


int shell_main(int argc_l, char **argv_l)
{
	int opt, interactive=FALSE;
	FILE *input = stdin;
	argc = argc_l;
	argv = argv_l;

	/* These variables need re-initializing when recursing */
	shell_context = 0;
	cwd=NULL;
	local_pending_command = NULL;
	close_me_head = NULL;
	job_list.head = NULL;
	job_list.fg = NULL;
#ifdef BB_FEATURE_SH_ENVIRONMENT
	last_bg_pid=1;
	last_return_code=1;
	show_x_trace=FALSE;
#endif

	if (argv[0] && argv[0][0] == '-') {
		FILE *prof_input;
		prof_input = fopen("/etc/profile", "r");
		if (!prof_input) {
			// printf( "Couldn't open file '/etc/profile'\n");
		} else {
			int tmp_fd = fileno(prof_input);
			mark_open(tmp_fd);	
			/* Now run the file */
			busy_loop(prof_input);
			fclose(prof_input);
			mark_closed(tmp_fd);
		}
	}

	while ((opt = getopt(argc_l, argv_l, "cxi")) > 0) {
		switch (opt) {
			case 'c':
				input = NULL;
				if (local_pending_command != 0)
					error_msg_and_die("multiple -c arguments");
				local_pending_command = xstrdup(argv[optind]);
				optind++;
				argv = argv+optind;
				break;
#ifdef BB_FEATURE_SH_ENVIRONMENT
			case 'x':
				show_x_trace = TRUE;
				break;
#endif
			case 'i':
				interactive = TRUE;
				break;
			default:
				show_usage();
		}
	}
	/* A shell is interactive if the `-i' flag was given, or if all of
	 * the following conditions are met:
	 *	  no -c command
	 *    no arguments remaining or the -s flag given
	 *    standard input is a terminal
	 *    standard output is a terminal
	 *    Refer to Posix.2, the description of the `sh' utility. */
	if (argv[optind]==NULL && input==stdin &&
			isatty(fileno(stdin)) && isatty(fileno(stdout))) {
		interactive=TRUE;
	}
	if (interactive==TRUE) {
		/* Looks like they want an interactive shell */
		printf( "\n\nBusyBox v%s (%s) Built-in shell (lash)\n", BB_VER, BB_BT);
		printf( "Enter 'help' for a list of built-in commands.\n\n");
	} else if (local_pending_command==NULL) {
		input = xfopen(argv[optind], "r");
		mark_open(fileno(input));  /* be lazy, never mark this closed */
	}

	/* initialize the cwd -- this is never freed...*/
	cwd=(char*)xmalloc(sizeof(char)*_MAX_LINE+1);
	getcwd(cwd, sizeof(char)*_MAX_LINE);

#ifdef BB_FEATURE_CLEAN_UP
	atexit(free_memory);
#endif

#ifdef BB_FEATURE_COMMAND_EDITING
	cmdedit_set_initial_prompt();
#else
	PS1 = NULL;
	PS2 = "> ";
#endif
	
	return (busy_loop(input));
}

