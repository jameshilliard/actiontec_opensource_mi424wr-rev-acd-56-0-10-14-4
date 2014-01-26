TOPDIR=../
include $(TOPDIR)Rules.mak
LIBC=$(TOPDIR)libc.a

LIBUTIL=libutil.a

LIB_STATIC=$(LIBUTIL)
LIB_SHARED=libutil.so
LIB_SHARED_MAJORNAME=$(LIB_SHARED).$(MAJOR_VERSION)
LIB_SHARED_FULLNAME=libutil-$(VERSION).so

A_TARGET=$(LIBUTIL)

O_OBJS_$(LIBUTIL)=login.o login_tty.o logout.o logwtmp.o openpty.o
ifeq ($(strip $(UCLIBC_HAS_MMU)),y)
  O_OBJS_$(LIBUTIL)+=forkpty.o
endif

include $(ULIBCMK)

$(LIB_SHARED_FULLNAME): $(LIB_STATIC)
	$(LD) $(EXTRA_LDFLAGS) -soname=$(LIB_SHARED_MAJORNAME) \
	  -o $@ --whole-archive $^ \
	  --no-whole-archive $(TOPDIR)/libc/misc/internals/interp.o \
	  -L$(TOPDIR)/lib -lc

