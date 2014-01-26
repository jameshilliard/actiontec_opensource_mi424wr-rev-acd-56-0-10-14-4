TOPDIR=../
include $(TOPDIR)Rules.mak

LIB_STATIC=libc.a
LIB_SHARED=libc.so
LIB_SHARED_MAJORNAME=$(LIB_SHARED).$(MAJOR_VERSION)
LIB_SHARED_FULLNAME=libuClibc-$(VERSION).so

SUBDIRS=inet misc pwd_grp stdio string termios signal stdlib sysdeps unistd

# Check if the target architecture has a version script for
# libc, and if so, include it when linking.
VERSION_SCRIPT=$(shell if [ -f sysdeps/linux/$(TARGET_ARCH)/libc.map ] ; then \
  echo "--version-script sysdeps/linux/$(TARGET_ARCH)/libc.map"; fi)

ifeq ($(strip $(UCLIBC_CTOR_DTOR)),y)
  CRTOBJS="../../lib/crti.o ../../lib/crt0.o ../../lib/crtn.o"
else
  CRTOBJS="../../lib/crt0.o"
endif

ifeq ($(strip $(ADD_LIBGCC_FUNCTIONS)),y)
  LIBGCC_NEED=./tmp/libgcc-need.a
else
  LIBGCC=
endif

include $(ULIBCMK)

$(LIB_SHARED_FULLNAME): $(LIB_STATIC) $(LIBGCC_NEED)
	$(LD) $(EXTRA_LDFLAGS) $(VERSION_SCRIPT) \
	  -soname=$(LIB_SHARED_MAJORNAME) \
	  -o $@ --whole-archive $^ $(TOPDIR)/libc/misc/internals/interp.o \
	  --no-whole-archive -init __uClibc_init $(LIBGCC)

$(LIBGCC_NEED): $(LIBC)
	@rm -rf tmp
	@mkdir tmp
	$(AR) rv $@
	(cd tmp && CC=$(CC) LD=$(LD) NM=$(NM) AR=$(AR) \
	  LIBGCC=$(LIBGCC) CRTOBJS=$(CRTOBJS) \
	  /bin/sh $(TOPDIR)../extra/scripts/get-needed-libgcc-objects.sh)

