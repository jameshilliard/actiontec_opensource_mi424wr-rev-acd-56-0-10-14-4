TOPDIR=../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

O_OBJS_$(LIBC)=lockf.o

ifeq ($(strip $(UCLIBC_HAS_LFS)),y)
  O_OBJS_$(LIBC)+=lockf64.o
endif

include $(RGMK)

