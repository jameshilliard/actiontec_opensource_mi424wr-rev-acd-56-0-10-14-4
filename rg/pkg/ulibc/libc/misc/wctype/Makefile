TOPDIR=../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

MSRC=wctype.c
MOBJ=iswalnum.o iswalpha.o iswcntrl.o iswdigit.o iswgraph.o iswlower.o \
  iswprint.o iswpunct.o iswspace.o iswupper.o iswxdigit.o towlower.o \
  towupper.o iswblank.o wctype.o iswctype.o wctrans.o towctrans.o

CFLAGS+=-DL_$*

OTHER_OBJS=$(MOBJ)
O_OBJS_$(LIBC)+=$(OTHER_OBJS)

include $(RGMK)

$(MOBJ): $(MSRC)
