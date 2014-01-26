TOPDIR=../../
include $(TOPDIR)Rules.mak

LDSO_FULLNAME=ld-uClibc-$(VERSION).so

LIB_STATIC=
LIB_SHARED=
LIB_SHARED_FULLNAME=$(LDSO_FULLNAME)
LIB_SHARED_MAJORNAME=$(UCLIBC_LDSO)

LDSO_OBJS=ldso.o #hash.o readelflib1.o $(TARGET_ARCH)/elfinterp.o
ASRC=$(wildcard $(TARGET_ARCH)/*.S)
AOBJS=$(patsubst %.S,%.o, $(ASRC))
LDSO_OBJS+=$(AOBJS)

CLEAN+=$(LDSO_OBJS)

# Unresolved warnings:
# "use of memory input without lvalue in asm operand 7 is deprecated"
ifeq ($(LIBC_ARCH),mips)
  CFLAGS_REMOVE_ldso.o+=-Werror
endif
ifeq ($(LIBC_ARCH),powerpc)
  CFLAGS_REMOVE_ldso.o+=-Werror
endif

CFLAGS+=-DUCLIBC_TARGET_PREFIX=\"$(TARGET_PREFIX)\" -DUCLIBC_DEVEL_PREFIX=\"$(DEVEL_PREFIX)\" -DUCLIBC_BUILD_DIR=\"$(shell cd $(TOPDIR) && pwd)\"

LDSO_LDFLAGS=$(EXTRA_LDFLAGS) -shared --warn-common --export-dynamic --sort-common -z combreloc --discard-locals --discard-all
 
ifeq ($(strip $(TARGET_ARCH)),cris)
  LDSO_LDFLAGS+=-mcrislinux
endif

ifneq ($(strip $(DODEBUG)),y)
  LDSO_LDFLAGS+=-s
endif

ifeq ($(strip $(SUPPORT_LD_DEBUG)),y)
  CFLAGS+=-D__SUPPORT_LD_DEBUG__
endif

ifeq ($(strip $(SUPPORT_LD_DEBUG_EARLY)),y)
  CFLAGS+=-D__SUPPORT_LD_DEBUG_EARLY__
endif

ifeq ($(strip $(FORCE_SHAREABLE_TEXT_SEGMENTS)),y)
  CFLAGS+=-DFORCE_SHAREABLE_TEXT_SEGMENTS
endif

CFLAGS+=-I. -I./$(TARGET_ARCH) -I../libdl

ARCHCONFIG_FIRST_TASKS=ldso.h

include $(ULIBCMK)

$(LDSO_FULLNAME): $(LIBGCC) $(LDSO_OBJS)
	$(LD) $(LDSO_LDFLAGS) -e _dl_boot -soname=$(UCLIBC_LDSO) \
	  -o $@ $(LDSO_OBJS) $(LIBGCC)

ldso.h:
	@echo "const char *_dl_progname=\""$(UCLIBC_LDSO)"\";" > ldso.h
	@echo "#include \"$(TARGET_ARCH)/elfinterp.c\"" >> ldso.h

