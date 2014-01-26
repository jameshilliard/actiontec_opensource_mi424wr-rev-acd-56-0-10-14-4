TOPDIR=../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

MSRC=time.c
MOBJ=asctime.o asctime_r.o clock.o ctime.o ctime_r.o gmtime.o gmtime_r.o \
  localtime.o localtime_r.o mktime.o strftime.o strptime.o tzset.o \
  _time_t2tm.o __time_tm.o _time_mktime.o dysize.o timegm.o

ifeq ($(strip $(UCLIBC_HAS_FLOATS)),y)
  MOBJ+=difftime.o 
endif

O_OBJS_$(LIBC)=adjtime.o ftime.o

CFLAGS+=-DL_$*

OTHER_OBJS=$(MOBJ)
O_OBJS_$(LIBC)+=$(OTHER_OBJS)

include $(RGMK)

$(MOBJ): $(MSRC)

