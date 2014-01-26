TOPDIR=../../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

O_OBJS_$(LIBC)=__longjmp.o vfork.o clone.o setjmp.o bsd-setjmp.o \
  bsd-_setjmp.o sigrestorer.o mmap64.o brk.o syscall.o ioperm.o sigaction.o

MSRC=crt0.S
MOBJ=crt0.o crt1.o gcrt1.o

CFLAGS+=-DL_$(subst -,_,$*)

OTHER_OBJS=$(MOBJ)
OTHER_TARGETS+=$(OTHER_OBJS)

SAFECFLAGS=$(subst " -g",,$(CFLAGS))

ifeq ($(strip $(UCLIBC_PROFILING)),y)
  FIRST_TASKS+=gmon-start.S
  CLEAN+=gmon-start.S
endif

INSTALL_OTHER=$(MOBJ)

include $(ULIBCMK)

$(MOBJ): $(MSRC)

gmon-start.S: ../common/gmon-start.c
	$(CC) $(SAFECFLAGS) -c $< -S -o $*.S
