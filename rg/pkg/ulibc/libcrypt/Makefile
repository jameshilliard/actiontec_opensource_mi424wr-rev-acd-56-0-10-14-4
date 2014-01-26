TOPDIR=../
include $(TOPDIR)Rules.mak

LIBCRYPT=libcrypt.a

LIB_STATIC=$(LIBCRYPT)
LIB_SHARED=libcrypt.so
LIB_SHARED_MAJORNAME=$(LIB_SHARED).$(MAJOR_VERSION)
LIB_SHARED_FULLNAME=libcrypt-$(VERSION).so

A_TARGET=$(LIBCRYPT)

O_OBJS_$(LIBCRYPT)=crypt.o des.o md5.o

include $(ULIBCMK)

$(LIB_SHARED_FULLNAME): $(LIB_STATIC)
	$(LD) $(EXTRA_LDFLAGS) -soname=$(LIB_SHARED_MAJORNAME) \
	  -o $@ --whole-archive $^ \
	  --no-whole-archive $(TOPDIR)/libc/misc/internals/interp.o \
	  -L$(TOPDIR)/lib -lc

