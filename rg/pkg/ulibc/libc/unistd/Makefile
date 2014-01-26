TOPDIR=../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

O_OBJS_$(LIBC)=execl.o execlp.o execv.o execvep.o execvp.o execle.o \
  sleep.o usleep.o getpass.o sysconf.o getlogin.o fpathconf.o confstr.o \
  pathconf.o swab.o usershell.o getopt.o getsubopt.o

ifeq ($(UCLIBC_HAS_MMU),y)
  O_OBJS_$(LIBC)+=daemon.o
endif

include $(RGMK)

