TOPDIR=../../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

O_OBJS_$(LIBC)=waitpid.o getdnnm.o gethstnm.o getcwd.o ptrace.o mkfifo.o \
  setegid.o wait.o getpagesize.o seteuid.o wait3.o setpgrp.o getdtablesize.o \
  create_module.o cmsg_nxthdr.o longjmp.o open64.o ftruncate64.o mmap64.o \
  truncate64.o getrlimit64.o setrlimit64.o creat64.o llseek.o pread_write.o \
  _exit.o sync.o getdirname.o sendfile64.o xstatconv.o getdents.o \
  getdents64.o vfork.o

ifneq ($(strip $(EXCLUDE_BRK)),y)
  O_OBJS_$(LIBC)+=sbrk.o
endif
ifeq ($(strip $(UCLIBC_PROFILING)),y)
  O_OBJS_$(LIBC)+=gmon.o
endif

MSRC=syscalls.c
MOBJ=$(shell ./list_syscalls.sh)

CFLAGS+=-DL_$*

OTHER_OBJS=$(MOBJ)
O_OBJS_$(LIBC)+=$(OTHER_OBJS)

CRT_OBJS=crti.o crtn.o
INSTALL_OTHER=$(CRT_OBJS)
CLEAN+=$(CRT_OBJS)

CFLAGS+=-fno-builtin
ifeq ($(strip $(UCLIBC_PROFILING)),y)
  CFLAGS+=-DGMON_SUPPORT
endif
SAFECFLAGS=$(subst -g,,$(CFLAGS))

include $(ULIBCMK)

$(MOBJ): $(MSRC)

ifeq ($(strip $(UCLIBC_CTOR_DTOR)),y)
initfini.S: initfini.c
	$(CC) $(SAFECFLAGS) -I$(TOPDIR)include -c initfini.c -S -o initfini.S

crti.S crtn.S: initfini.S $(TOPDIR)/extra/scripts/initfini.awk
	$(AWK) -f $(TOPDIR)extra/scripts/initfini.awk initfini.S
else
#Just create an empty o files
$(CRT_OBJS):
	$(AR) $(ARFLAGS) $@
endif
