TOPDIR=../../
include $(TOPDIR)Rules.mak

#Adjust the soname version to avoid namespace collisions with glibc's libpthread
LIBTHREAD_DB=libthread_db.a

LIB_STATIC+=$(LIBTHREAD_DB)
LIB_SHARED+=libpthread_db.so
LIB_SHARED_MAJORNAME+=libpthread_db.so.$(MAJOR_VERSION)
LIB_SHARED_FULLNAME+=libpthread_db-$(VERSION).so

ifeq ($(strip $(UCLIBC_CTOR_DTOR)),y)
  ifeq ($(strip $(DOPIC)),y)
    START_FILES=$(TOPDIR)lib/crti.o $(LIBGCC_DIR)crtbeginS.o
    END_FILES=$(LIBGCC_DIR)crtendS.o $(TOPDIR)lib/crtn.o
  else
    START_FILES=$(TOPDIR)lib/crti.o $(LIBGCC_DIR)crtbegin.o
    END_FILES=$(LIBGCC_DIR)crtend.o $(TOPDIR)lib/crtn.o
  endif
endif

A_TARGET=$(LIBTHREAD_DB)

# set up system dependencies include dirs (NOTE: order matters!)
PTDIR=$(TOPDIR)libpthread/linuxthreads/
SYSDEPINC=-I$(PTDIR)sysdeps/unix/sysv/linux -I$(PTDIR)sysdeps/pthread \
  -I$(PTDIR)sysdeps/unix/sysv -I$(PTDIR)sysdeps/unix/unix \
  -I$(PTDIR)sysdeps/$(TARGET_ARCH) -I$(PTDIR)sysdeps \
  -I$(TOPDIR)libc/sysdeps/linux/$(TARGET_ARCH)
  
CFLAGS+=$(SYSDEPINC) -DLIBPTHREAD_SO="\"libpthread.so.$(MAJOR_VERSION)\""

O_OBJS_$(LIBTHREAD_DB)=td_init.o td_log.o td_ta_delete.o td_ta_get_nthreads.o \
  td_ta_get_ph.o td_ta_map_id2thr.o td_ta_map_lwp2thr.o	td_ta_new.o \
  td_ta_thr_iter.o td_ta_tsd_iter.o td_thr_get_info.o td_thr_getfpregs.o \
  td_thr_getgregs.o td_thr_getxregs.o td_thr_getxregsize.o td_thr_setfpregs.o \
  td_thr_setgregs.o td_thr_setprio.o td_thr_setsigpending.o td_thr_setxregs.o \
  td_thr_sigsetmask.o td_thr_tsd.o td_thr_validate.o td_thr_dbsuspend.o \
  td_thr_dbresume.o td_ta_setconcurrency.o td_ta_enable_stats.o \
  td_ta_reset_stats.o td_ta_get_stats.o td_ta_event_addr.o \
  td_thr_event_enable.o td_thr_set_event.o td_thr_clear_event.o \
  td_thr_event_getmsg.o td_ta_set_event.o td_ta_event_getmsg.o \
  td_ta_clear_event.o td_symbol_list.o td_thr_tls_get_addr.o

ifeq ($(LIBC_ARCH),mips)
  # We get warning EF_MIPS_ARCH_32 redefined, both in:
  # pkg/ulibc/include/elf.h
  # os/linux-2.4/include/asm-mips/elf.h
  CFLAGS_REMOVE_td_thr_tls_get_addr.o+=-Werror
endif

include $(ULIBCMK)

libpthread_db-$(VERSION).so: libthread_db.a
	$(LD) $(EXTRA_LDFLAGS) -soname=libthread_db.so.$(MAJOR_VERSION) \
	  -o $@ $(START_FILES) --whole-archive $^ \
	  --no-whole-archive $(TOPDIR)/libc/misc/internals/interp.o \
	  -L$(TOPDIR)/lib -lc $(END_FILES)

