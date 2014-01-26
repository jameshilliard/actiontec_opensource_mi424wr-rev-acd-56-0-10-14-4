RGSRC=../../
include $(RGSRC)/envir.mak

TARGET+=iwconfig iwlist iwpriv iwspy iwgetid iwevent ifrename

O_OBJS_iwconfig+=iwconfig.o
O_OBJS_iwlist+=iwlist.o
O_OBJS_iwpriv+=iwpriv.o
O_OBJS_iwspy+=iwspy.o
O_OBJS_iwgetid+=iwgetid.o
O_OBJS_iwevent+=iwevent.o
O_OBJS_ifrename+=ifrename.o

ifndef CONFIG_DYN_LINK
  LIBIW=libiw.a
  A_TARGET+=libiw.a
else
  LIBIW=libiw.so
  SO_TARGET+=libiw.so
endif

O_OBJS_$(LIBIW)+=iwlib.o

LDLIBS:=-liw -lm $(LDLIBS)
LDFLAGS+=-L.

include $(RGMK) 
