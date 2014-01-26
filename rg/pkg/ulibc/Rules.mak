# Rules.make for uClibc
#
# Copyright (C) 2000 by Lineo, inc.
# Copyright (C) 2000-2002 Erik Andersen <andersen@uclibc.org>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Library General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU Library General Public License for more
# details.
#
# You should have received a copy of the GNU Library General Public License
# along with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
RGSRC=$(TOPDIR)../../
include $(RGSRC)/envir.mak
include $(TOPDIR)/Config

# XXX Remove when change all Makefiles
STRIPTOOL=$(STRIP)
HOSTCFLAGS=$(LOCAL_CFLAGS)

#uClibc version
MAJOR_VERSION=0
MINOR_VERSION=9
SUBLEVEL=20
VERSION=$(MAJOR_VERSION).$(MINOR_VERSION).$(SUBLEVEL)

# Ensure consistent sort order, 'gcc -print-search-dirs' behavior, etc. 
LC_ALL=C
export MAJOR_VERSION MINOR_VERSION SUBLEVEL VERSION LC_ALL TOPDIR

LIBC=$(TOPDIR)libc/libc.a

UCLIBC_LDSO:=ld-uClibc.so.$(MAJOR_VERSION)

# check if we have nawk, otherwise user awk
# XXX Move to envir.mak
AWK:=$(shell if [ -x /usr/bin/nawk ]; then echo "/usr/bin/nawk"; \
  else echo "/usr/bin/awk"; fi)

TARGET_ARCH:=$(LIBC_ARCH)

ARFLAGS:=r

# Some nice CFLAGS/LDFLAGS to work with
CFLAGS+=-fno-builtin -nostdinc -D_LIBC -I$(TOPDIR)include -I.
LDFLAGS+=-shared --warn-common --warn-once -z combreloc
EXTRA_LDFLAGS+=$(ENDIAN_LDFLAGS) -shared --warn-common --warn-once -z combreloc

ifeq ($(DODEBUG),y)
  CFLAGS+=-g
  STRIPTOOL:=true -Since_we_are_debugging
else
  LDFLAGS+=-s
endif

# Sigh, some stupid versions of gcc can't seem to cope with
# '-iwithprefix include'
#CFLAGS+=-iwithprefix include
CFLAGS+=$(shell $(TARGET_CC) -print-search-dirs | sed -ne "s/install: *\(.*\)/-I\1include/gp")

ifneq ($(DOASSERTS),y)
  CFLAGS+=-DNDEBUG
endif

ifeq ($(HAVE_SHARED),y)
  LIBRARY_CACHE:=#-DUSE_CACHE
  ifeq ($(BUILD_UCLIBC_LDSO),y)
    LDSO:=$(TOPDIR)lib/$(UCLIBC_LDSO)
    DYNAMIC_LINKER:=$(SHARED_LIB_LOADER_PATH)/$(UCLIBC_LDSO)
    BUILD_DYNAMIC_LINKER:=$(shell cd $(TOPDIR) && pwd)/lib/$(UCLIBC_LDSO)
  else
    LDSO:=$(SYSTEM_LDSO)
    DYNAMIC_LINKER:=/lib/$(strip $(subst ",, $(notdir $(SYSTEM_LDSO))))
    BUILD_DYNAMIC_LINKER:=/lib/$(strip $(subst ",, $(notdir $(SYSTEM_LDSO))))
  endif
endif

CFLAGS_NOPIC=$(CFLAGS)
ifeq ($(DOPIC),y)
  ifeq ($(strip $(TARGET_ARCH)),cris)
    CFLAGS+=-fpic -mlinux
  else
    CFLAGS+=-fPIC
  endif
endif

LIBGCC_CFLAGS?=$(CFLAGS)
LIBGCC:=$(shell $(TARGET_CC) $(LIBGCC_CFLAGS) -print-libgcc-file-name)
LIBGCC_DIR:=$(dir $(LIBGCC))

ifdef CONFIG_BINFMT_SHARED_FLAT
  # For the shared version of this, we specify no stack and its library ID
  FLTFLAGS+=-s 0
  LIBID=1
  export LIBID FLTFLAGS
  SHARED_TARGET = lib/libc
endif

ULIBCMK=$(TOPDIR)ulibc.mak
