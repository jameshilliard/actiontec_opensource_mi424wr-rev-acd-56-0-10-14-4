TOPDIR=../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

O_OBJS_$(LIBC)=pwent.o getpwnam.o getpwuid.o putpwent.o getpw.o fgetpwent.o \
  __getgrent.o grent.o getgrnam.o getgrgid.o fgetgrent.o initgroups.o \
  __getpwent_r.o

ifeq ($(strip $(HAS_SHADOW)),y)
  O_OBJS_$(LIBC)+=lckpwdf.o spent.o getspnam.o getspuid.o putspent.o \
    sgetspent.o fgetspent.o __getspent_r.o __sgetspent_r.o
endif

include $(RGMK)

