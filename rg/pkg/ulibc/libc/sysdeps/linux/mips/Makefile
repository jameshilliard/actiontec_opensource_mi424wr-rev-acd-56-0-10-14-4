TOPDIR=../../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

O_OBJS_$(LIBC)=bsd-_setjmp.o bsd-setjmp.o setjmp.o clone.o fork.o syscall.o \
  pipe.o __longjmp.o  brk.o vfork.o setjmp_aux.o _mmap.o __syscall_error.o \
  cacheflush.o

MSRC=crt0.S
MOBJ=crt0.o crt1.o

CFLAGS+=-DL_$(subst -,_,$*)

OTHER_OBJS=$(MOBJ)
OTHER_TARGETS+=$(OTHER_OBJS)

SAFECFLAGS=$(subst " -g",,$(CFLAGS))

CFLAGS_REMOVE__mmap.o+=-Werror

INSTALL_OTHER=$(MOBJ)

include $(ULIBCMK)

archconfig::
	$(RG_LN) $(CURDIR)/sgidefs.h $(TOPDIR)/include/

$(MOBJ): $(MSRC)
