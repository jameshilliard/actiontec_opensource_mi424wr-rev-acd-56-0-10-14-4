TOPDIR=../../
include $(TOPDIR)Rules.mak
TARGET_CC = $(TOPDIR)extra/gcc-uClibc/$(TARGET_ARCH)-uclibc-gcc

ARCHCONFIG_FIRST_TASKS=elf.h

#build ldd
LOCAL_TARGET+=ldd
#TARGET+=ldd.target
O_OBJS_ldd=ldd.o
O_OBJS_ldd.target=ldd.o
LDDFLAGS=-DUCLIBC_TARGET_PREFIX=\"$(TARGET_PREFIX)\" \
  -DUCLIBC_DEVEL_PREFIX=\"$(DEVEL_PREFIX)\" \
  -DUCLIBC_BUILD_DIR=\"$(shell cd $(TOPDIR) && pwd)\" \
  -DUCLIBC_LDSO=\"$(UCLIBC_LDSO)\" -I.

CFLAGS_ldd.target+=$(LDDFLAGS)
LOCAL_CFLAGS+=$(LDDFLAGS)

ifeq ($(strip $(LDSO_LDD_SUPPORT)),y)
  CFLAGS_ldd+=-D__LDSO_LDD_SUPPORT
endif

#build readelf
ifeq ($(OSTYPE),linux)
  LOCAL_TARGET+=readelf
  #TARGET+=readelf.target
  O_OBJS_readelf=readelf.o
  O_OBJS_readelf.target=readelf.o
endif

ifneq ($(strip $(LIBRARY_CACHE)),)
  LOCAL_TARGET+=ldconfig
  O_OBJS_ldconfig+=readsoname.o ldconfig.o
endif

include $(RGMK)

elf.h: $(TOPDIR)include/elf.h
	ln -fs $< $@
