TOPDIR=../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

MSRC=locale.c
MOBJ=setlocale.o localeconv.o _locale_init.o nl_langinfo.o

CFLAGS+=-DL_$*

OTHER_OBJS=$(MOBJ)
O_OBJS_$(LIBC)+=$(OTHER_OBJS)

# XXX locale_data.o is a link from the $(TOPDIR)/extra/locale
ifeq ($(strip $(UCLIBC_HAS_LOCALE)),y)
  SUBDIRS+=$(TOPDIR)/extra/locale
  O_OBJS_$(LIBC)=+locale_data.o
endif

include $(RGMK)

$(MOBJ): $(MSRC)
