TOPDIR=../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

MSRC=sem.c
MOBJ=semget.o semctl.o semop.o

MSRC2=shm.c
MOBJ2=shmat.o shmctl.o shmdt.o shmget.o

MSRC3=msgq.c
MOBJ3=msgctl.o msgget.o msgrcv.o msgsnd.o

O_OBJS_$(LIBC)=ftok.o

CFLAGS+=-DL_$*

OTHER_OBJS=$(MOBJ) $(MOBJ2) $(MOBJ3)
O_OBJS_$(LIBC)+=$(OTHER_OBJS)

include $(RGMK)

$(MOBJ): $(MSRC)
$(MOBJ2): $(MSRC2)
$(MOBJ3): $(MSRC3)

