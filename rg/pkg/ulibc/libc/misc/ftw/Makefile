TOPDIR=../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

MSRC=ftw.c
MOBJ=ftw.o ftw64.o

CFLAGS+=-DL_$*

OTHER_OBJS=$(MOBJ)
O_OBJS_$(LIBC)+=$(OTHER_OBJS)

include $(RGMK)

$(MOBJ): $(MSRC)

