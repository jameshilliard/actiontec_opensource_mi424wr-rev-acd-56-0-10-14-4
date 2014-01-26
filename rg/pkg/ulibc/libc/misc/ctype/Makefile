TOPDIR=../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

MSRC=ctype.c
MOBJ=isalnum.o isalpha.o isascii.o iscntrl.o isdigit.o isgraph.o \
  islower.o isprint.o ispunct.o isspace.o isupper.o isxdigit.o \
  isxlower.o isxupper.o toascii.o tolower.o toupper.o isblank.o \
  __isctype_loc.o

CFLAGS+=-DL_$*

O_OBJS_$(LIBC)=junk.o

OTHER_OBJS=$(MOBJ)
O_OBJS_$(LIBC)+=$(OTHER_OBJS)

include $(RGMK)

$(MOBJ): $(MSRC)

