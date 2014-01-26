/****************************************************************************
 *
 * rg/pkg/util/rg_error.c
 * 
 * Copyright (C) Jungo LTD 2004
 * 
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General 
 * Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 * Developed by Jungo LTD.
 * Residential Gateway Software Division
 * www.jungo.com
 * info@jungo.com
 */

#include <rg_os.h>

#define RG_ERROR_INTERNALS
#include <util/rg_error.h>

char *rg_error_level_str[] = { "EMERGENCY", "ALERT", "CRITICAL", "ERROR", 
    "WARNING", "NOTICE", "INFO", "DEBUG"};
static int (*main_addr)(int argc, char *argv[]);
static char *main_name;
#define MAX_PROC_NAME 16
static char proc_name[MAX_PROC_NAME] = "unknown";
rg_error_level_t rg_error_console_level; /* console logging from here down */
static int inited;
static int reboot_on_exit; /* reboot on LDOEXIT */
static int reboot_on_panic; /* reboot on LDOREBOOT */

#if defined(__KERNEL__) && defined(__OS_LINUX__)
#define vsnprintf(str, num, fmt, va) vsprintf(str, fmt, va)
#endif

#if defined(__OS_VXWORKS__) || defined(__OS_LINUX__) && !defined(__KERNEL__)
/* Headers for VxWorks and Linux user-mode */
#define OS_INCLUDE_IO
#define OS_INCLUDE_STD
#define OS_INCLUDE_STRING
#define OS_INCLUDE_SIGNAL
#include <os_includes.h>
#endif

#ifdef __OS_VXWORKS__
#include <taskLib.h>

static int mt_task_id;
#endif

#if defined(__OS_LINUX__) && defined(__KERNEL__)
#define printf(params...) printk(params)
#define malloc(sz) kmalloc(sz, GFP_KERNEL)
#define free(p) kfree(p)
#define fprintf(fp, params...) printk(params)
#endif

static int is_mt_context(void)
{
#ifdef __OS_VXWORKS__
    return taskIdSelf() == mt_task_id;
#elif defined(__KERNEL__)
    return 0;
#else
    return 1;
#endif
}

void rg_error_set_mt_id(void)
{
#ifdef __OS_VXWORKS__
    mt_task_id = taskIdSelf();
#endif
}

#ifdef __KERNEL__ 
#include <kos/kos.h>
#ifdef __OS_LINUX__
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/in.h>
#endif
#else
#include <signal.h>
#include <util/rg_print.h>
#endif

/* set the below pointers from the outside, to change
 * the default behavior on exit/reboot
 */
void (*rg_error_reboot_func)(void);
void (*rg_error_exit_func)(void);

void rg_error_reboot(void)
{
    if (rg_error_reboot_func)
	rg_error_reboot_func();
#if defined(__OS_LINUX__) && defined(__KERNEL__)
    /* in linux kernel dont reboot, for now */
#if 0
    machine_restart();
#endif
#else
#ifdef HAVE_KILL
   /* signal the init process (main_task or init) to reboot. */
   kill(1, SIGINT);
#else
   /* This code generates three types of exception: 
    * division by 0 - handled by FPU/Emulator, 
    * unaligned access (float at wrong alignment) - for non-MMU out there
    * segmentation  - 1 is not legal at most MMU platforms.
    */
   *(float *)1 = 1 / 0.0;
#endif
#endif
}

void rg_error_exit(void)
{
    if (rg_error_exit_func)
	rg_error_exit_func();
#if defined(__OS_LINUX__) && defined(__KERNEL__)
#else
    exit(1);
#endif
}

/* External declaration of main task entry func for stack trace */

void *begin_addr;

void strace_init(void)
{
    int temp;
    begin_addr = (void *)&temp + sizeof(void *);
}

static void strace_write(void)
{
    int *end_addr, row_counter;
    char *str;
   
    if (!(str = malloc(100 * sizeof(char))))
	return;
    if (!begin_addr)
	return;
    end_addr = (int *)&end_addr;
    printf("Stack trace");
    if (main_name)
	printf(" (%s()=%p)\n", main_name ? main_name : "none", main_addr);

    sprintf(str, "User-mode stack:(0x%08x to 0x%08x)", (int)end_addr,
	(int)begin_addr);
    for (row_counter = 0; (void *)end_addr <= begin_addr;
	end_addr++, row_counter++)
    {
	if ((row_counter % 8) == 0)
	{
	    printf("%s\n", str);
	    sprintf(str, "%04x:", ((int)end_addr & 0x0000FFE0));
	    if (row_counter == 0)
	    {
		int i = 0;

		for (; i < (((unsigned int)end_addr & 0x0000001F) / 4); i++)
		    sprintf(str + strlen(str), "         ");
	    }
	}
	sprintf(str + strlen(str), " %08x", (unsigned int)*end_addr);
    }
    printf("%s\n", str);
    free(str);
}

/* This function is called from signal handlers, therefore special
 * care was taken NOT to use static buffers! 
 */
static char *get_proc_name(void)
{
    static char pname[MAX_PROC_NAME];

#if !defined(__KERNEL__) && defined(CONFIG_PROC_FS)
    /* In Linux user-mode we can get the process name from /proc/self/status */

    /* Skip "Name:\t" */
#define PROC_STATUS_SKIP 6
    char skip_buf[PROC_STATUS_SKIP];
    int fd;
    int i;
    
    fd = open("/proc/self/status", O_RDONLY);
    if (fd<0)
	goto Error;
    /* Skip "Name:\t" */
    i = read(fd, skip_buf, PROC_STATUS_SKIP);
    if (i != PROC_STATUS_SKIP)
	goto Error;
    /* Linux holds maximum 15 chars of the name */
    i = read(fd, pname, MAX_PROC_NAME-1);
    if (i < 0)
	goto Error;
    pname[MAX_PROC_NAME-1] = 0;
    if (strchr(pname, '\n'))
	*strchr(pname, '\n') = 0;
    goto Exit;
Error:    
    strcpy(pname, "unknown");
Exit:
    if (fd>=0)
	close(fd);
#else
    strcpy(pname, "unknown");
#endif
    return pname;
}

static void check_inited(void)
{
    if (inited)
	return;
    rg_error_init_default(LNOTICE);
}

void rg_error_init(int _reboot_on_exit, int _reboot_on_panic,
    int strace, char *process, rg_error_level_t console_level,
    int (*_main_addr)(int, char *[]), char *_main_name)
{
    if (inited)
	rg_error(LEXIT, "rg_error_init: already inited");
    inited = 1;
    if (strace)
	strace_init();
    if (process)
    {
	strncpy(proc_name, process, MAX_PROC_NAME);
	proc_name[MAX_PROC_NAME-1] = 0;
    }
    else
	strcpy(proc_name, get_proc_name());
    rg_error_console_level = console_level;
    main_addr = _main_addr;
    main_name = _main_name;
    reboot_on_exit = _reboot_on_exit;
    reboot_on_panic = _reboot_on_panic;
}

void rg_error_uninit(void)
{
}

#ifndef __KERNEL__
static void rg_error_uninit_default(void)
{
    rg_error_unregister(default_error_cb, NULL);
}
#endif

void rg_error_init_default(rg_error_level_t level)
{
    rg_error_init_nodefault();
    rg_error_register(0, level | LCONSOLE, default_error_cb, NULL);
#ifndef __KERNEL__
    atexit(rg_error_uninit_default);
#endif
}

void rg_error_init_nodefault(void)
{
    rg_error_init(0, 0, 0, NULL, LERR, NULL, NULL);
}

/* TODO: This logic can (and should) move to syslog task along with all
 * rg_error funcs.
 */
typedef struct rg_error_notify_t {
    struct rg_error_notify_t *next;
    rg_error_func_t func;
    void *data;
    rg_error_level_t level;
    int sequence;
} rg_error_notify_t;

static rg_error_notify_t *rg_error_notify;

void default_error_cb(void *data, char *msg, rg_error_level_t level)
{
    fprintf(stderr, "%s: %s\n", rg_error_level_str[level & LLEVEL_MASK], msg);
}

void rg_error_register(int sequence, rg_error_level_t level,
    rg_error_func_t func, void *data)
{
    rg_error_notify_t **n, *next;
    check_inited();
    for (n = &rg_error_notify; *n && (*n)->sequence<=sequence; n = &(*n)->next);
    next = *n;
    if (!(*n = (rg_error_notify_t *)malloc(sizeof(**n))))
    {
	rg_error_f(LEXIT, "failed malloc");
	return;
    }
    memset(*n, 0, sizeof(**n));
    (*n)->func = func;
    (*n)->data = data;
    (*n)->level = level;
    (*n)->sequence = sequence;
    (*n)->next = next;
}

/* Return the number of functions unregistred */
int rg_error_unregister(rg_error_func_t func, void *data)
{
    rg_error_notify_t **cur;
    int freed = 0;

    for (cur = &rg_error_notify; *cur;)
    {
	if (data == (*cur)->data && func == (*cur)->func)
	{
	    rg_error_notify_t *tmp = *cur;
	    *cur = tmp->next;
	    free(tmp);
	    freed++;
	}
	else
	    cur = &(*cur)->next;
    }
    return freed;
}

void log_msg_deliver(rg_error_level_t level, char *msg)
{
    rg_error_notify_t *notify;

    /* Going through the notify list */
    for (notify = rg_error_notify; notify; notify = notify->next)
    {
	if ((notify->level & LLEVEL_MASK) >= (level & LLEVEL_MASK) ||
	    level & notify->level & LFLAGS_MASK)
	{
	    notify->func(notify->data, msg, level);
	}
    }

#ifndef __KERNEL__
    if (level & LCONSOLE || level <= rg_error_console_level)
	console_printf("%s\n", msg);
#endif
}

#ifdef __KERNEL__
int (*rg_error_logdev_hook)(char *msg, int exact_len);
#endif

/* deliver through logdev */
static void log_dev_enqueue(rg_error_level_t level, char *msg)
{
#ifdef __KERNEL__ 
    char pri_msg[8];

    if (!rg_error_logdev_hook)
	return;
    sprintf(pri_msg, "<%d>", (unsigned short)level);
    if ((*rg_error_logdev_hook)(pri_msg, strlen(pri_msg)) < strlen(pri_msg))
	return;
    (*rg_error_logdev_hook)(msg, -1);
#endif
}

#define MAX_ERR_SIZE 1024
static int _rg_vperror(rg_error_level_t level, const char *str_error,
    const char *format, va_list ap)
{
    static char msg[MAX_ERR_SIZE];
    static char format_after_m[MAX_ERR_SIZE]; /* will hold format after 
						* expansion of '%m'
						*/
#ifndef __KERNEL__
    int saved_errno = errno;
    sigset_t aset, oset;    
#else
    kos_interrupt_flags_t f;
#endif

    check_inited();
#ifndef __KERNEL__
    /* Blocking of the signals is done to protect the static buffer msg[] */
    sigfillset(&aset);
    sigprocmask(SIG_SETMASK, &aset, &oset);
#else
    f = kos_save_flags_cli();
#endif

    if (format)
    {
	/* handle %m */
	char *p = format_after_m;
	char *p_end = format_after_m + sizeof(format_after_m) - 1;
	const char *format_last = format;
#ifndef __KERNEL__
	char *format_cur;

	while ((format_cur = strstr(format_last, "%m")))
	{
	    /* copy segment up to '%m' */
	    int chars_to_copy = format_cur - format_last;
	    if (chars_to_copy > p_end - p)
		chars_to_copy = p_end - p;
	    memcpy(p, format_last, chars_to_copy);
	    p += chars_to_copy;
	    /* add error string */
	    strncpy(p, strerror(saved_errno), p_end - p);
	    *p_end = 0;
	    p += strlen(p);
	    format_last = format_cur + 2; /* skip '%m' */
	}
#endif

	/* copy last segment */
	strncpy(p, format_last, p_end - p);
	*p_end = 0;
	p += strlen(p);

	if (str_error && p < p_end)
	{
	    strncpy(p, ": ", p_end-p);
	    *p_end = 0;
	    p += strlen(p);
	    strncpy(p, str_error, p_end-p);
	    *p_end = 0;
	}

	vsnprintf(msg, sizeof(msg), format_after_m, ap);
    }
    else
	strcpy(msg, "error in `rg_error': no message specified");

    if (is_mt_context())
	log_msg_deliver(level, msg); /* direct delivery */
    else
	log_dev_enqueue(level, msg); /* deliver through logdev */

    if (level & LDOEXIT)
    {
	if (reboot_on_exit)
	{
	    strace_write();
	    rg_error_reboot();
	}
	else
	    rg_error_exit();
    }
    else if (level & LDOREBOOT)
    {
	strace_write();
	if (reboot_on_panic)
	    rg_error_reboot();
	else
	    rg_error_exit();
    }

#ifndef __KERNEL__ 
    sigprocmask(SIG_SETMASK, &oset, 0);
#else
    kos_restore_flags(f);
#endif
    return -1;
}
 
int rg_error(rg_error_level_t level, const char *format, ...)
{
    va_list ap;
    int ret;
    
    va_start(ap, format);
    ret = _rg_vperror(level, NULL, format, ap);
    va_end(ap);

    return ret;
}

int rg_verror(rg_error_level_t level, const char *format, va_list ap)
{
    return _rg_vperror(level, NULL, format, ap);
}

#if !defined(__KERNEL__) || defined(__USERMODE_IN_KERNEL__)
int rg_perror(rg_error_level_t level, const char *format, ...)
{
    va_list ap;
    int ret;
    
    va_start(ap, format);
    ret = _rg_vperror(level, strerror(errno), format, ap);
    va_end(ap);

    return ret;
}

int rg_vperror(rg_error_level_t level, const char *format, va_list ap)
{
    return _rg_vperror(level, strerror(errno), format, ap);
}
#endif

