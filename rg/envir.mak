# all is the first target.
all:

# We can do either 'make archconfig' or 'make config'
DOING_MAKE_CONFIG:=$(DOING_MAKE_CONFIG)
ifeq ($(MAKECMDGOALS),archconfig)
  DOING_MAKE_CONFIG:=y
endif
ifeq ($(MAKECMDGOALS),config)
  DOING_MAKE_CONFIG:=y
endif

# check whether makeflags for config were defined
ifeq ($(DOING_MAKE_CONFIG)-$(IN_RGSRC_ROOT),y-y)
  ifeq ($(DIST),)
    # fetch makeflags from license file, if possible
    ifneq ($(LIC),)
      $(shell rm -f /tmp/rg_config.mak)
      $(shell $(RGSRC)/pkg/license/lic_rg makeflags-lines "$(LIC)" \
        2>/dev/null 1>/tmp/rg_config.mak)
      include /tmp/rg_config.mak
      $(shell rm -f /tmp/rg_config.mak)
      EXTRA_MAKEFLAGS:=$(shell $(RGSRC)/pkg/license/lic_rg makeflags "$(LIC)" \
      2>/dev/null)
    endif
  endif
  ifeq ($(DIST),)
    $(error DIST not defined)
  endif
endif
.PHONY: /tmp/rg_config.mak

# handle user pre-defined BUILD directory: set BUILDDIR accordingly
ifdef BUILD
  export BUILD
  BUILDDIR:=$(if $(filter /%,$(BUILD)),,$(shell cd $(RGSRC); pwd)/)$(BUILD)
endif

# handle BUILDDIR
ifdef BUILDDIR
  # we are already in BUILDDIR, or the user specified a specific BUILD dir
  IS_BUILDDIR:=$(if $(findstring $(BUILDDIR),$(CURDIR)),y)
  ifndef IS_BUILDDIR
    CREATE_BUILDDIR_LINK:=y
  endif
else
  # we are in the initial make source directory, we need to move
  # to BUILDDIR and respawn make
  IS_BUILDDIR:=$(if $(findstring $(RGSRC)/build,$(CURDIR)),y)
  ifndef IS_BUILDDIR
    ifdef DOING_MAKE_CONFIG
      ifeq ($(CURDIR),$(RGSRC))
       CREATE_BUILDDIR_LINK:=y
       BUILDDIR:=$(RGSRC)/build.$(DIST)
      endif
    endif
  endif
endif

BUILDDIR_LINK:=$(RGSRC)/build

CREATE_BUILDDIR_LINK_=$(shell mkdir -p $1; ln -sfn $1 $(BUILDDIR_LINK))
ifdef CREATE_BUILDDIR_LINK
    $(call CREATE_BUILDDIR_LINK_,$(BUILDDIR))
else
  ifndef BUILDDIR
    ifneq ($(wildcard $(BUILDDIR_LINK)),)
      BUILDDIR:=$(shell readlink $(BUILDDIR_LINK))
    else
      ifeq ($(DIST),)
        $(error DIST undefined (Cannot find $(BUILDDIR_LINK)). Run make config)
      else
        $(error Cannot find $(BUILDDIR_LINK). Run make config)
      endif
    endif
  endif
endif

export RGSRC:=$(if $(filter /%,$(RGSRC)),$(RGSRC),$(shell cd $(RGSRC) && pwd))

JPKG_EXPORTED_FILES+=Makefile
JPKG_EXPORTED_IF_EXIST+=envir.subdirs.mak

JPKG_DIR=$(BUILDDIR)/jpkg/rg

export PWD_SRC:=$(if $(IS_BUILDDIR),$(CURDIR:$(BUILDDIR)%=$(RGSRC)%),$(CURDIR))
export PWD_BUILD:=$(if $(IS_BUILDDIR),$(CURDIR),$(CURDIR:$(RGSRC)%=$(BUILDDIR)%))
export PWD_JPKG:=$(CURDIR:$(if $(IS_BUILDDIR),$(BUILDDIR),$(RGSRC))%=$(JPKG_DIR)%)

export PWD_REL:=$(if $(filter $(CURDIR),$(BUILDDIR)),.,$(subst $(BUILDDIR)/,,$(CURDIR)))
export JPKG_CUR_DIR=$(JPKG_DIR)/$(PWD_REL)

RGMK=$(RGSRC)/rg.mak

# internal functions for implementing PATH_UP
PWD_REL_LIST=$(subst /,! ,$1)
PWD_REL_NUM=$(words $(call PWD_REL_LIST,$1))
PWD_REL_MINUS_ONE=$(words $(wordlist 2,$(call PWD_REL_NUM,$1),$(call PWD_REL_LIST,$1)))
PATH_UP_REMOVE_LAST=$(wordlist 1,$(call PWD_REL_MINUS_ONE,$1),$(call PWD_REL_LIST,$1))

# usage: "$(call PATH_UP,a/b/c/d)" gives "a/b/c"
PATH_UP=$(subst !,,$(subst ! ,/,$(call PATH_UP_REMOVE_LAST,$1)))

ifdef IS_BUILDDIR

# PWD_REL=pkg/util --> PATH_UP_START=./pkg/util --> ./pkg/util ./pkg .
# PWD_REL=. --> PATH_UP_START=./ --> .
PATH_UP_START:=$(if $(filter .,$(PWD_REL)),./,./$(PWD_REL))
include $(RGSRC)/path_up_recursive.mak

# Include envir.subdirs.mak in all the parent directories up to RGSRC
-include $(addsuffix /envir.subdirs.mak,$(addprefix $(RGSRC)/,$(PATH_UP_LIST)))

VPATH:=$(if $(NEED_VPATH),$(PWD_SRC))

else  # IS_BUILDDIR
# we are in the source directory. we need to move to the build directory
all $(filter-out echovar distclean, $(MAKECMDGOALS)): cd_to_builddir

# This is the catch-all target for the SRCDIR
# we need it since the real target can only be built in BUILDDIR and we still
# need to do something in SRCDIR
%:;

endif # IS_BUILDDIR

CHECK_CONFIGURE:=$(if $(DOING_MAKE_CONFIG),,check_configure)

CONFIG_STARTED_FILE=$(BUILDDIR)/.make_config_running

FIX_VPATH_CFLAGS_=$(if $(findstring $(BUILDDIR),$1),$(subst $(BUILDDIR),$(RGSRC),$1),\
  $(if $(findstring $(RGSRC),$1),$(subst $(RGSRC),$(BUILDDIR),$1),\
  $(1:-I%=-I$(VPATH)/%)))

FIX_VPATH_CFLAGS=$(foreach f,$1,$f $(if $(filter -I%,$f),$(call FIX_VPATH_CFLAGS_,$f))) \
  $(if $(IS_LINK_DIR),,-I$(CURDIR) -I$(PWD_SRC))

FIX_VPATH_LDFLAGS=$(foreach f,$1,$f $(if $(filter -L$(RGSRC)%,$f),\
$(subst $(RGSRC),$(BUILDDIR),$f),\
$(if $(filter -L$(BUILDDIR)%,$f),$(subst $(BUILDDIR),$(RGSRC),$f))))

CONFIG_FILE=$(BUILDDIR)/rg_config.mk
RG_CONFIG_H=$(BUILDDIR)/rg_config.h
RG_CONFIG_C=$(BUILDDIR)/pkg/util/rg_c_config.c

LOOP:=$(RGSRC)/loop.mak
export COPY_DB=$(RGSRC)/copy_db.mak
export COPY_DB_ENVIR=$(RGSRC)/copy_db_envir.mak
export CONFIGURATION_FILE=$(BUILDDIR)/config

-include $(CONFIG_FILE)

MKDIR=mkdir -p

export LD_LIBRARY_PATH:=$(LD_LIBRARY_PATH):.

export RG_LIBPTHREAD_VERSION NEEDED_SYMBOLS OS_TARGET NET_TOOLS_MAKE_FILE \
  BOOTLDR_COMPRESSED_DISK GENROMFS CC CXX LD AR AS NM RANLIB STRIP SIZE \
  OBJCOPY OBJDUMP ARCH FLTHDR ARCH LIBC_ARCH RG_CPU TOOLS_PREFIX \
  COMPRESSED_DISK TARGET_MACHINE COMPRESSED_RAMDISK I386_TOOLS_PREFIX \
  MODFS_DISK COMPRESSED_INIT_DISK RG_LIB_VERSION LIBC_DIR_PREFIX PATH LDFLAT \
  PROD_KERNEL_IMAGE RG_BIN RG_INCLUDE RG_LIB ENV BASH_ENV TARGET_CC \
  TARGET_CXX CC_FOR_BUILD CXX_FOR_BUILD BUILDDIR DOING_MAKE_CONFIG \
  CROSS_COMPILE CONFIG_RG_JPKG_DEBUG CONFIG_BINFMT_FLAT CONFIG_BLK_DEV_SD \
  CONFIG_CRAMFS_BLKSZ CONFIG_CRAMFS_DYN_BLOCKSIZE CONFIG_CRAMFS_FS \
  CONFIG_EXT2_FS CONFIG_GLIBC CONFIG_HW_ST_20190 CONFIG_INCAIP \
  CONFIG_INCAIP_DSP CONFIG_INCAIP_KEYPAD CONFIG_INCAIP_LEDMATRIX \
  CONFIG_INCAIPPWM CONFIG_INCAIPSSC CONFIG_INSURE CONFIG_MTD \
  CONFIG_RAMDISK_SIZE CONFIG_RG_CONSOLE_DEVICE CONFIG_RG_CRAMFS_COMP_METHOD \
  CONFIG_RG_FILESERVER CONFIG_RG_INITFS_CRAMFS CONFIG_RG_INITFS_RAMDISK \
  CONFIG_RG_KERNEL_NEEDED_SYMBOLS CONFIG_RG_MODFS CONFIG_ZSP400 \
  CONFIG_RG_NONCOMPRESSED_USERMODE_IMAGE CONFIG_RG_OS CONFIG_RG_RGLOADER \
  CONFIG_RG_THREADS CONFIG_RG_UML CONFIG_ROMFS_FS CONFIG_SIMPLE_CRAMDISK \
  CONFIG_SIMPLE_RAMDISK CONFIG_USB_PRINTER CONFIG_VINETIC TARGET_ENDIANESS \
  CONFIG_JFFS2_FS_FEATURE CONFIG_RG_HW CONFIG_KSFLASH CONFIG_BLK_DEV_MD

# We dont define CFLAGS in rg_config.mk because the kernel dose not like it.
# Will be fixed with B2846
CFLAGS=$(CFLAGS_ENVIR)
LDFLAGS=$(LDFLAGS_ENVIR)

-include $(RGSRC)/pkg/include/libc_config.make

NORMAL_TARGETS=$(strip $(sort $(TARGET) $(O_TARGET) $(SO_TARGET) $(A_TARGET) \
  $(MOD_TARGET)))
LOCAL_TARGETS=$(strip $(sort $(LOCAL_TARGET) $(LOCAL_CXX_TARGET) \
  $(LOCAL_A_TARGET))) $(LOCAL_O_TARGET)
ALL_TARGETS=$(LOCAL_TARGETS) $(NORMAL_TARGETS) $(OTHER_TARGETS)
# Note: I'm using _O_OBJS_% variables that are only calculated in rg.mak. 
# These calculated variables can be used only in and after including rg.mak
ALL_LOCAL_OBJS=$(sort $(foreach o,$(LOCAL_TARGETS),$(_O_OBJS_$o)))
ALL_TARGET_OBJS=$(sort $(foreach o,$(NORMAL_TARGETS),$(_O_OBJS_$o)) $(O_OBJS) \
  $(_OTHER_OBJS) $(sO_OBJS) \
  $(foreach o,$(MOD_TARGET),$(OX_OBJS_$o)))
ALL_OBJS=$(ALL_LOCAL_OBJS) $(ALL_TARGET_OBJS) $(filter %.o,$(OTHER_TARGETS))
ALL_PRODS=$(ALL_OBJS) $(ALL_TARGETS)

ifdef CONFIG_BINFMT_FLAT
  ALL_PRODS+=$(foreach t,$(TARGET),$t.elf.o)
  ALL_PRODS+=$(foreach t,$(TARGET),$t.gdb.o)
endif

A_TARGETS=$(A_TARGET) $(LOCAL_A_TARGET)

include $(RGSRC)/copy_db_envir.mak
-include $(BUILDDIR)/os/kernel/envir.mak
