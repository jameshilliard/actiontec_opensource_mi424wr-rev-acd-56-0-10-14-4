TOPDIR=../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

MSRC=termios.c
MOBJ=tcdrain.o tcflow.o tcflush.o tcsendbreak.o tcsetpgrp.o tcgetpgrp.o \
  isatty.o cfgetospeed.o cfgetispeed.o cfsetospeed.o cfsetispeed.o \
  cfmakeraw.o cfsetspeed.o

O_OBJS_$(LIBC)=tcgetattr.o tcgetsid.o tcsetattr.o ttyname.o

CFLAGS+=-DL_$*

OTHER_OBJS=$(MOBJ)
O_OBJS_$(LIBC)+=$(OTHER_OBJS)

include $(RGMK)

$(MOBJ): $(MSRC)

