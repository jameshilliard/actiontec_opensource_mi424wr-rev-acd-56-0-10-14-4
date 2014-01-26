TOPDIR=../../
include $(TOPDIR)Rules.mak

#Adjust the soname version to avoid namespace collisions with glibc's libpthread
LIBPTHREAD=libpthread.a

LIB_STATIC=$(LIBPTHREAD)
LIB_SHARED=libpthread.so
LIB_SHARED_MAJORNAME=libpthread.so.$(MAJOR_VERSION)
LIB_SHARED_FULLNAME=libpthread-$(VERSION).so

ifeq ($(strip $(UCLIBC_CTOR_DTOR)),y)
  ifeq ($(strip $(DOPIC)),y)
    START_FILES=$(TOPDIR)lib/crti.o $(LIBGCC_DIR)crtbeginS.o
    END_FILES=$(LIBGCC_DIR)crtendS.o $(TOPDIR)lib/crtn.o
  else
    START_FILES=$(TOPDIR)lib/crti.o $(LIBGCC_DIR)crtbegin.o
    END_FILES=$(LIBGCC_DIR)crtend.o $(TOPDIR)lib/crtn.o
  endif
endif

A_TARGET=$(LIBPTHREAD)

#This stuff will not compile without at least -O1
CFLAGS:=$(subst -O0,-O1,$(CFLAGS))

# set up system dependencies include dirs (NOTE: order matters!)
PTDIR=$(TOPDIR)libpthread/linuxthreads/
SYSDEPINC=-I$(PTDIR)sysdeps/unix/sysv/linux -I$(PTDIR)sysdeps/pthread \
  -I$(PTDIR)sysdeps/unix/sysv -I$(PTDIR)sysdeps/unix/unix \
  -I$(PTDIR)sysdeps/$(TARGET_ARCH) -I$(PTDIR)sysdeps \
  -I$(TOPDIR)libc/sysdeps/linux/$(TARGET_ARCH)
CFLAGS+=$(SYSDEPINC)

O_OBJS_$(LIBPTHREAD)=attr.o cancel.o condvar.o errno.o events.o join.o \
  lockfile.o manager.o mutex.o oldsemaphore.o pt-machine.o ptfork.o pthread.o \
  ptlongjmp.o rwlock.o semaphore.o signals.o specific.o spinlock.o \
  wrapsyscall.o #weaks.o

include $(ULIBCMK)

libpthread-$(VERSION).so: libpthread.a
	$(LD) $(EXTRA_LDFLAGS) -soname=libpthread.so.$(MAJOR_VERSION) \
	  -o $@ $(START_FILES) --whole-archive $^ \
	  --no-whole-archive $(TOPDIR)/libc/misc/internals/interp.o \
	  -L$(TOPDIR)/lib -lc $(END_FILES)

