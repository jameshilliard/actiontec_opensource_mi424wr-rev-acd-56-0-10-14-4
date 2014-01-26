TOPDIR=../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

PTDIR=$(TOPDIR)libpthread/linuxthreads/
SYSDEPINC=-I$(PTDIR)sysdeps/unix/sysv/linux -I$(PTDIR)sysdeps/pthread \
  -I$(PTDIR)sysdeps/unix/sysv -I$(PTDIR)sysdeps/unix/unix \
  -I$(PTDIR)sysdeps/$(TARGET_ARCH) -I$(PTDIR)sysdeps \
  -I$(TOPDIR)libc/sysdeps/linux/$(TARGET_ARCH)

CFLAGS+=$(SYSDEPINC) -D_GNU_SOURCE

O_OBJS_$(LIBC)=no-tsd.o weaks.o

include $(RGMK)

