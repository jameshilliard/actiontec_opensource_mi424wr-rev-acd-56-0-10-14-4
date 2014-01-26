TOPDIR=../
include $(TOPDIR)Rules.mak

LIBNSL=libnsl.a

LIB_STATIC=$(LIBNSL)
LIB_SHARED=libnsl.so
LIB_SHARED_MAJORNAME=$(LIB_SHARED).$(MAJOR_VERSION)
LIB_SHARED_FULLNAME=libnsl-$(VERSION).so

A_TARGET=$(LIBNSL)

O_OBJS_$(LIBNSL)=nsl.o

include $(ULIBCMK)

$(LIB_SHARED_FULLNAME): $(LIB_STATIC)
	$(LD) $(EXTRA_LDFLAGS) -soname=$(LIB_SHARED_MAJORNAME) \
	  -o $@ --whole-archive $^ \
	  --no-whole-archive $(TOPDIR)/libc/misc/internals/interp.o \
	  -L$(TOPDIR)/lib -lc

