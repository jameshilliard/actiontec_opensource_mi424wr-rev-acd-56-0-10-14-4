TOPDIR=../../
include $(TOPDIR)Rules.mak

LIBDL=libdl.a

LIB_STATIC=$(LIBDL)
LIB_SHARED=libdl.so
LIB_SHARED_MAJORNAME=$(LIB_SHARED).$(MAJOR_VERSION)
LIB_SHARED_FULLNAME=libdl-$(VERSION).so

A_TARGET=$(LIBDL)

O_OBJS_$(LIBDL)=dlib.o #../ldso/$(TARGET_ARCH)/resolve.o

ifeq ($(LIBC_ARCH),mips)
  # Unresolved warnings:
  # "use of memory input without lvalue in asm operand 7 is deprecated"
  CFLAGS_REMOVE_dlib.o+=-Werror
endif


CFLAGS+=-DUCLIBC_TARGET_PREFIX=\"$(TARGET_PREFIX)\" -DUCLIBC_DEVEL_PREFIX=\"$(DEVEL_PREFIX)\" -DUCLIBC_BUILD_DIR=\"$(shell cd $(TOPDIR) && pwd)\"
ifeq ($(strip $(SUPPORT_LD_DEBUG)),y)
  CFLAGS+=-D__SUPPORT_LD_DEBUG__
endif
CFLAGS+=-I../ldso -I../ldso/$(TARGET_ARCH)

include $(ULIBCMK)

$(LIB_SHARED_FULLNAME): $(LIB_STATIC)
	$(LD) $(EXTRA_LDFLAGS) -soname=$(LIB_SHARED_MAJORNAME) \
	  -o $@ --whole-archive $^ \
	  --no-whole-archive $(TOPDIR)/libc/misc/internals/interp.o \
	  -L$(TOPDIR)/lib -lc
