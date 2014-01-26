TOPDIR=../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

O_OBJS_$(LIBC)=statvfs.o fstatvfs.o

ifeq ($(strip $(UCLIBC_HAS_LFS)),y)
  O_OBJS_$(LIBC)+=fstatfs64.o statfs64.o statvfs64.o fstatvfs64.o
endif

include $(RGMK)

