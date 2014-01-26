RGSRC=../..
include $(RGSRC)/envir.mak

# Makefile for busybox
#
# Copyright (C) 1999,2000,2001 Erik Andersen <andersee@debian.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#

PROG      := busybox
VERSION   := 0.50
BUILDTIME := $(shell TZ=UTC date --utc "+%Y.%m.%d-%H:%M%z")
export VERSION

# With a modern GNU make(1) (highly recommended, that's what all the
# developers use), all of the following configuration values can be
# overridden at the command line.  For example:
#   make CROSS=powerpc-linux- BB_SRC_DIR=$HOME/busybox PREFIX=/mnt/app

# If you want to add some simple compiler switches (like -march=i686),
# especially from the command line, use this instead of CFLAGS directly.
# For optimization overrides, it's better still to set OPTIMIZATION.
CFLAGS_EXTRA =
 
# If you want a static binary, turn this on.
DOSTATIC = $(if $(CONFIG_VALGRIND),true,false)

# Set the following to `true' to make a debuggable build.
# Leave this set to `false' for production use.
# eg: `make DODEBUG=true tests'
# Do not enable this for production builds...
DODEBUG = false

# Setting this to `true' will cause busybox to directly use the system's
# password and group functions.  Assuming you use GNU libc, when this is
# `true', you will need to install the /etc/nsswitch.conf configuration file
# and the required libnss_* libraries. This generally makes your embedded
# system quite a bit larger... If you leave this off, busybox will directly
# use the /etc/password, /etc/group files (and your system will be smaller, and
# I will get fewer emails asking about how glibc NSS works).  Enabling this adds
# just 1.4k to the binary size (which is a _lot_ less then glibc NSS costs),
# Most people will want to leave this set to false.
USE_SYSTEM_PWD_GRP = true

# This enables compiling with dmalloc ( http://dmalloc.com/ )
# which is an excellent public domain mem leak and malloc problem
# detector.  To enable dmalloc, before running busybox you will
# want to first set up your environment.
# eg: `export DMALLOC_OPTIONS=debug=0x14f47d83,inter=100,log=logfile`
# Do not enable this for production builds...
DODMALLOC = false

# If you want large file summit support, turn this on.
# This has no effect if you don't have a kernel with lfs
# support, and a system with libc-2.1.3 or later.
# Some of the programs that can benefit from lfs support
# are dd, gzip, mount, tar, and mkfs_minix.
# LFS allows you to use the above programs for files
# larger than 2GB!
ifdef CONFIG_RG_LARGE_FILE_SUPPORT
DOLFS = true
endif

# If you have a "pristine" source directory, point BB_SRC_DIR to it.
# Experimental and incomplete; tell the mailing list
# <busybox@opensource.lineo.com> if you do or don't like it so far.
BB_SRC_DIR = 

# If you are running a cross compiler, you may want to set this
# to something more interesting, like "powerpc-linux-".
STRIPTOOL := $(STRIP)

# To compile vs uClibc, just use the compiler wrapper built by uClibc...
# This make things very easy?  Everything should compile and work as
# expected these days...
#CC = ../uClibc/extra/gcc-uClibc/gcc-uClibc-i386

# To compile vs some other alternative libc, you may need to use/adjust
# the following lines to meet your needs...
#
# If you are using Red Hat 6.x with the compatible RPMs (for developing under
# Red Hat 5.x and glibc 2.0) uncomment the following.  Be sure to read about
# using the compatible RPMs (compat-*) at http://www.redhat.com !
#LIBCDIR=/usr/i386-glibc20-linux
#
# The following is used for libc5 (if you install altgcc and libc5-altdev
# on a Debian system).  
#LIBCDIR=/usr/i486-linuxlibc1
#
# For other libraries, you are on your own...
#LDFLAGS+=-nostdlib
#LIBRARIES = $(LIBCDIR)/lib/libc.a -lgcc
#CROSS_CFLAGS+=-nostdinc -I$(LIBCDIR)/include -I$(GCCINCDIR)
#GCCINCDIR = $(shell gcc -print-search-dirs | sed -ne "s/install: \(.*\)/\1include/gp")

# use '-Os' optimization if available, else use -O2
OPTIMIZATION := $(shell if $(CC) -Os -S -o /dev/null -xc /dev/null >/dev/null 2>&1; \
    then echo "-Os"; else echo "-O2" ; fi)

WARNINGS = -Wall

#
#--------------------------------------------------------
# If you're going to do a lot of builds with a non-vanilla configuration,
# it makes sense to adjust parameters above, so you can type "make"
# by itself, instead of following it by the same half-dozen overrides
# every time.  The stuff below, on the other hand, is probably less
# prone to casual user adjustment.
# 

CFLAGS+=$(CONFIG_BUSYBOX_CFLAGS)
ifeq ($(strip $(DOLFS)),true)
    # For large file summit support
    CFLAGS+=-D_FILE_OFFSET_BITS=64
endif
ifeq ($(strip $(DODMALLOC)),true)
    # For testing mem leaks with dmalloc
    CFLAGS+=-DDMALLOC
    LIBRARIES = -ldmalloc
    # Force debug=true, since this is useless when not debugging...
    DODEBUG = true
endif
ifeq ($(strip $(DODEBUG)),true)
    CFLAGS  += $(WARNINGS) -g -D_GNU_SOURCE
    LDFLAGS += -Wl,-warn-common
    STRIP    =
else
    CFLAGS  += $(WARNINGS) $(OPTIMIZATION) -D_GNU_SOURCE
    LDFLAGS += -Wl,-warn-common
    STRIP    = $(STRIPTOOL) --remove-section=.note --remove-section=.comment $(PROG)
endif
ifeq ($(strip $(DOSTATIC)),true)
    LDFLAGS += --static
    #
    #use '-ffunction-sections -fdata-sections' and '--gc-sections' (if they 
    # work) to try and strip out any unused junk.  Doesn't do much for me, 
    # but you may want to give it a shot...
    #
    #ifeq ($(shell $(CC) -ffunction-sections -fdata-sections -S \
    #	-o /dev/null -xc /dev/null 2>/dev/null && $(LD) \
    #			--gc-sections -v >/dev/null && echo 1),1)
    #	CFLAGS += -ffunction-sections -fdata-sections
    #	LDFLAGS += --gc-sections
    #endif
endif

ifndef $(PREFIX)
    PREFIX = `pwd`/_install
endif

# Additional complications due to support for pristine source dir.
# Include files in the build directory should take precedence over
# the copy in BB_SRC_DIR, both during the compilation phase and the
# shell script that finds the list of object files.
# Work in progress by <ldoolitt@recycle.lbl.gov>.
#
ifneq ($(strip $(BB_SRC_DIR)),)
    VPATH = $(BB_SRC_DIR)
endif

# the -I- remove all rg_gcc path and it can't find all included files
# so as long as we use -nostdinc we have to define DISABLE_VPATH=yes

# We need to set APPLET_SOURCES to something like
#   $(shell busybox.sh Config.h)
# but in a manner that works with VPATH and BB_SRC_DIR.
# Possible ways to approach this:
#
#   1. Explicitly search through .:$(VPATH) for busybox.sh and config.h,
#      then $(shell $(BUSYBOX_SH) $(CONFIG_H) $(BB_SRC_DIR))
#
#   2. Explicity search through .:$(VPATH) for slist.mk,
#      then $(shell $(MAKE) -f $(SLIST_MK) VPATH=$(VPATH) BB_SRC_DIR=$(BB_SRC_DIR))
#
#   3. Create slist.mk in this directory, with commands embedded in
#      a $(shell ...) command, and $(MAKE) it immediately.
#
#   4. Use a real rule within this makefile to create a file that sets 
#      APPLET_SOURCE_LIST, then include that file.  Has complications
#      with the first trip through the makefile (before processing the
#      include) trying to do too much, and a spurious warning the first
#      time make is run.
#
# This is option 3:
#
#APPLET_SOURCES = $(shell \
#   echo -e 'all: busybox.sh Config.h\n\t@ $$(SHELL) $$^ $$(BB_SRC_DIR)' >slist.mk; \
#   make -f slist.mk VPATH=$(VPATH) BB_SRC_DIR=$(BB_SRC_DIR) \
#)
# And option 4:

TARGET=busybox

ifndef CONFIG_OPENRG
  TARGET+=init
endif

COMMON_OBJS=busybox.o messages.o usage.o utility.o

CFLAGS_init=-DCONFIG_BB_INIT

# Create a list of C files needed for each target.
COMMAND_FILE=variable.mak
FOR_EACH=$(TARGET)
INDEX_VARIABLE=APPLET_SOURCES_$(INDEX)
INDEX_VAL=$(shell CC="$(CC_FOR_BUILD)" \
	  CFLAGS_CONFIG_SPEC="$(CFLAGS_$(INDEX))" \
	  BUILDDIR=$(BUILDDIR) $(PWD_SRC)/busybox.sh \
	  $(PWD_SRC)/Config.h $(BB_SRC_DIR))
include $(LOOP)

# Convert the list of C files to a list of objects (with the correct suffix for
# each target).
FOR_EACH=$(TARGET)
INDEX_VARIABLE=O_OBJS_$(INDEX)
INDEX_VAL=$(APPLET_SOURCES_$(INDEX):%.c=%_$(INDEX).o) \
	  $(COMMON_OBJS:%.o=%_$(INDEX).o)
include $(LOOP)

# Suffix not needed (and disturbs) for main target.
O_OBJS_busybox:=$(O_OBJS_busybox:%_busybox.o=%.o)

FOR_EACH=$(O_OBJS_init)
INDEX_VARIABLE=CFLAGS_$(INDEX)
INDEX_VAL=$(CFLAGS_init)
include $(LOOP)

OTHER_OBJS+=$(O_OBJS_init)

CFLAGS    += $(CROSS_CFLAGS) -fno-builtin
CFLAGS    += -DBB_VER='"$(VERSION)"'
CFLAGS    += -DBB_BT='"$(BUILDTIME)"'
ifdef BB_INIT_SCRIPT
    CFLAGS += -DINIT_SCRIPT='"$(BB_INIT_SCRIPT)"'
endif
CFLAGS    += -DUSE_SYSTEM_PWD_GRP

# Put user-supplied flags at the end, where they
# have a chance of winning.
CFLAGS += $(CFLAGS_EXTRA)

OTHER_TARGETS = busybox.links 

LDFLAGS	+= $(LIBRARIES) 
ifdef CONFIG_ARMNOMMU
  FLAT_STACK_SIZE_busybox=10240
endif

CLEAN += busybox.links core $(OBJECTS) $(LIBRARIES) $(PWD_LIB)

include $(RGMK)

ifdef IS_BUILDDIR

FOR_EACH=$(O_OBJS_init)
INDEX_DEP=$(INDEX:%_init.o=%.c)
include $(LOOP)

archconfig:: loop.h

ramdisk::
	$(call RAMDISK_MKDIR_FUNC,$(RAMDISK_RW_DIR)/bin)
	$(call RAMDISK_MKDIR_FUNC,$(RAMDISK_RO_DIR)/bin)
	@./install.sh $(PREFIX)

applet_source_list: busybox.sh Config.h
	(echo -n "APPLET_SOURCES := "; $(SHELL) $^ $(BB_SRC_DIR)) > $@

#	$(STRIP) -- not needed for RG, done later on

# Without VPATH, rule expands to "/bin/sh busybox.mkll Config.h applets.h"
# but with VPATH, some or all of those file names are resolved to the
# directories in which they live.
busybox.links: busybox.mkll Config.h applets.h
	- $(SHELL) $^ >$@

nfsmount.o cmdedit.o: %.o: %.h

loop.h: mk_loop_h.sh
	@ LINUX_SRC=$(RGSRC)/linux $(SHELL) $< > $@

endif
