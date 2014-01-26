TOPDIR=../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

MSRC=tsearch.c
MOBJ=tsearch.o tfind.o tdelete.o twalk.o tdestroy.o

MSRC2=lsearch.c
MOBJ2=lfind.o lsearch.o

MSRC3=insremque.c
MOBJ3=insque.o remque.o

MSRC4=hsearch_r.c
MOBJ4=hcreate_r.o hdestroy_r.o hsearch_r.o

O_OBJS_$(LIBC)=hsearch.o

CFLAGS+=-DL_$*

OTHER_OBJS=$(MOBJ) $(MOBJ2) $(MOBJ3) $(MOBJ4)
O_OBJS_$(LIBC)+=$(OTHER_OBJS)

include $(RGMK)

$(MOBJ): $(MSRC)
$(MOBJ2): $(MSRC2)
$(MOBJ3): $(MSRC3)
$(MOBJ4): $(MSRC4)
