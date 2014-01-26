TOPDIR=../
include $(TOPDIR)Rules.mak

LIBRESOLV=libresolv.a

LIB_STATIC=$(LIBRESOLV)
LIB_SHARED=libresolv.so
LIB_SHARED_MAJORNAME=$(LIB_SHARED).$(MAJOR_VERSION)
LIB_SHARED_FULLNAME=libresolv-$(VERSION).so

A_TARGET=$(LIBRESOLV)

O_OBJS_$(LIBRESOLV)=resolv.o

include $(ULIBCMK)

$(LIB_SHARED_FULLNAME): $(LIB_STATIC)
	$(LD) $(EXTRA_LDFLAGS) -soname=$(LIB_SHARED_MAJORNAME) \
	  -o $@ --whole-archive $^ \
	  --no-whole-archive $(TOPDIR)/libc/misc/internals/interp.o \
	  -L$(TOPDIR)/lib -lc

