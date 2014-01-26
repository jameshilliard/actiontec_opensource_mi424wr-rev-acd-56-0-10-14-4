#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/poll.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>
#include "test_tool.h"

#define MIN(a, b) ((a)<(b)?(a):(b))

int verify_file_content(char *name, char *buf, int len)
{
    int l = len;
    char *p = get_file_content(name, &l);
    int ret = 0;
    
    if (len!=l || memcmp(p, buf, l))
	goto Exit;
    ret = 1;
Exit:
    free(p);
    return ret;
}


int verify_link(char *lname, char *tname)
{
    char buf[MAX_PATH_SIZE];
    int r = readlink(lname, buf, sizeof(buf));
    if (r==-1)
	return 0;
    if (strncmp(tname, buf, r) || r!=strlen(tname))
	return 0;
    return 1;
}

void copy_file(char *src, char *dst)
{
    int sd = -1, dd = -1;
    struct stat st;
    char *buf = x_malloc(BUFSIZ);
    int rd, ret = 1;

    if(stat(src, &st))
	goto Exit;
    dd = open(dst, O_WRONLY|O_CREAT, st.st_mode);
    if (dd<0)
	goto Exit;
    sd = open(src, O_RDONLY);
    if (sd<0)
	goto Exit;
    
    while ((rd = read(sd, buf, BUFSIZ))>0)
    {
	if (rd != write(dd, buf, rd))
	    goto Exit;
    }
    ret = 0;
Exit:
    free(buf);
    if (sd>=0)
	close(sd);
    if (dd>=0)
	close(dd);
    if (ret)
	end_program(1, "copy files");
}

void remove_recursive(char *name)
{
    struct dirent *d;
    DIR *dir;
    struct stat st;

    if (lstat(name, &st))
	return;
    if((!S_ISDIR(st.st_mode)) || st.st_mode&S_IFLNK)
    {
	unlink(name);
	return;
    }
 
    dir = opendir(name);
    if (!dir)
	return;
    while((d = readdir(dir)))
    {
	char n[MAX_PATH_SIZE];
	if (!strcmp(d->d_name, ".") || !strcmp(d->d_name, ".."))
	    continue;
	strcpy(n, name);
	strcat(n, "/");
	strcat(n, d->d_name);
	remove_recursive(n);
    }
    closedir(dir);
    rmdir(name);
}

void create_path(char *name)
{
    char *f = strdup(name);
    char *t = f;
    int len;

    if (!f)
	end_program(1, "cannot malloc");
    len = strlen(f);
    while (t-f<len)
    {
	for (t++; *t!='/' && *t; t++);
	*t = 0;
	mkdir(f, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
	*t = '/';
    }
    *t = 0;
    free(f);
}

long time_diff(struct timeval *a, struct timeval *b)
{
    long sc = b->tv_sec - a->tv_sec;
    long usc = b->tv_usec - a->tv_usec;

    if (sc<0)
	sc += 24*60*60;

    return sc*1000000 + usc;
}

char *skip_begining_of_next_word(char* w)
{
    char *p;
    
    for(p=w; !isspace(*p) && *p; p++);
    for(; isspace(*p); p++);

    return p;
}

int run_and_compare(const char *command, const char *buf, int len)
{
    int l = len;
    char *out = activate(command, &l);
    int ret = 0;
    if (l!=len || memcmp(out, buf, l))
	ret = 1;

    free(out);
    return ret;
}


char *get_file_content(const char *name, int *len)
{
    int fd = open(name, O_RDONLY);
    char *r;
    int t, rd = 0;

    if (fd<0)
    {
	*len = 0;
	return NULL;
    }
    /* Need to read one byte more in reading of the file 
     * in order to find differences from expected length
     */
    *len += 1;
    r = x_malloc(*len);

    while (rd<*len)
    {
	t = read(fd, r+rd, MIN(*len-rd, BUFSIZ));
	if (t<=0)
	    break;
	rd += t;
    }
    
    *len = rd;
    close(fd);
    return r;
}

int read_buffer_from_fd(int fd, int *len, char *buf)
{
    int t, rd=0;
    int pr;
    int time = SLOW_TIME;
    struct pollfd pl;
    
    fcntl(fd, F_SETFL, O_NONBLOCK);
    while (rd < *len)
    {
	t = read(fd, buf+rd, MIN(*len, BUFSIZ));
	if (t>0)
	    rd += t;
	else
	{
	    if (rd < *len-1)
		time = SLOW_TIME;
	    else
		time = EXTRA_TIME;
	    pl.fd = fd;
	    pl.events = POLLIN|POLLPRI|POLLHUP;
	    pl.revents =  0;
	    pr = poll(&pl, 1, time);
	    
	    if (pl.revents & POLLHUP)
		break;
	    if (pr==0)
		break;
	}
    }
	    
    *len = rd;
    return 0;
}

void write_file(const char *name, const char *content, int len, int times,
    int flags)
{
    int fd, ret;
    const char *tmp;
    int rt;

    if(!flags)
	flags = S_IRUSR|S_IRGRP|S_IROTH;
    fd = open(name, O_CREAT|O_RDWR, flags);
    if (fd<0)
	end_program(1, "could not creat file (%s)", name);
    
    for(; times!=0; times--)
    {
	for (rt = len, tmp = (char*)content; rt > 0; rt -= ret, tmp += ret)
	{
	    ret = write(fd, tmp, rt);
	    if (ret<0)
	    {
		close(fd);
		end_program(1, "error writing to file (%s)", name);
	    }
	}
    }
    close(fd);
}


pid_t run_program(const char *cmd, int *fd_stdin, int *fd_stdout,
	int *fd_stderr, int *ci, int *co, int *ce)
{
    char *argv[MAX_ARGS];
    int argc = 0;
    char *tmp;
    volatile int dup2_failed = 0, exec_failed = 0;
    char *c, *f; 
    pid_t pid;

    int fdi[2], fdo[2], fde[2];
   
    if (fd_stdin)
    {
	if (pipe(fdi)<0)
	    end_program(1, "cannot pipe(in)");
	*fd_stdin = fdi[1];
	*ci = fdi[0];
    }

    if (fd_stdout)
    {
	if (pipe(fdo)<0)
	    end_program(1, "cannot pipe(out)");
	*fd_stdout = fdo[0];
	*co = fdo[1];
    }

    if (fd_stderr)
    {
	if (pipe(fde)<0)
	    end_program(1, "cannot pipe(err)");
	*fd_stderr = fde[0];
	*ce = fde[1];
    }

    f = c = x_malloc(strlen(cmd)+1);
    strcpy(c, cmd);

    while (*c)
    {
	argv[argc] = c;
	for (;*c && !isspace(*c); c++);
	for (tmp=c ; isspace(*c); c++);
	*tmp=0;
	 /* set special characters (usually interpeted by th shell) */
	if (!strcmp("@NEWLINE@", argv[argc]))
	    strcpy(argv[argc], "\n");
	if (!strcmp("@SPACE@", argv[argc]))
	    strcpy(argv[argc], " ");
	if (*argv[argc] == '\\')
	    argv[argc]++;
	argc++;
    }
    argv[argc] = NULL;

    pid = vfork();
    if (pid<0)
	end_program(1, "cannot vfork");
    
    if (pid==0)
    {
	dup2_failed = 1;
	if (fd_stderr)
	{
	    if (dup2(fde[1], 2)<0)
		_exit(1);
	    close(fde[0]);
	}
	if (fd_stdout)
	{
	    if (dup2(fdo[1], 1)<0)
		_exit(1);
	    close(fdo[0]);
	}
	if (fd_stdin)
	{
	    if (dup2(fdi[0], 0)<0)
		_exit(1);
	    close(fdi[1]);
	}
	
	dup2_failed = 0;
	execvp(argv[0], argv);
	exec_failed = 1;
	_exit(1);
    }
    
    free(f);
    
    if (exec_failed)
	end_program(1, "exec failed");
    if (dup2_failed)
	end_program(1, "dup2 failed");

    return pid;
}

int check_dir(const char *name)
{
    struct stat st;
    if (stat(name, &st))
	return 0;
    return S_ISDIR(st.st_mode);
}

char *activate_e(const char *cmd, int *outlen, char **errbuf, int *errlen, 
    char *inbuf, int *inlen)
{
    char *outbuf = NULL;
    pid_t pid;

    int fdo, fde, fin;
    int ci, co, ce;
    int *pfdo = NULL, *pfde = NULL, *pfin = NULL;
    

    if (inlen)
	pfin = &fin;
    
    if (outlen)
    {
	outbuf = x_malloc(*outlen+=1);
	pfdo = &fdo;
    }
    
    if (errlen)
    {
	if(!(*errbuf = malloc(*errlen+=1)))
	{
	    free(outbuf);
	    end_program(1, "cannot malloc");
	}
	pfde = &fde;
    }

    pid = run_program(cmd, pfin, pfdo, pfde, &ci, &co, &ce);

/* this utility is currently limited to first writing the input and only then
 * waiting for output
 */
    if (inlen)
    {
	int rt = 0;
	int t;
	while(rt < *inlen)
	{
	    t = write(fin, inbuf+rt, *inlen-rt);
	    if (t<0)
		break;
	    rt += t;
	}
	*inlen = rt;
	close(fin);
	close(ci);
    } 
    
    if (outlen)
    {
	read_buffer_from_fd(fdo, outlen, outbuf);
	close(fdo);
	close(co);
    }
    
    if (errlen)
    {
	read_buffer_from_fd(fde, errlen, *errbuf);
	close(fde);
	close(ce);
    }

    waitpid(pid, NULL, 0);
    return outbuf;
}

char *activate(const char *cmd, int *len)
{
    return activate_e(cmd, len, NULL, NULL, NULL, NULL);
}

