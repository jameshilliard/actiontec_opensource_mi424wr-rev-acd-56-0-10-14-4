TOPDIR=../
include $(TOPDIR)Rules.mak

# We don't use any ld shared utils
#SUBDIRS=util

ifeq ($(strip $(BUILD_UCLIBC_LDSO)),y)
  SUBDIRS+=ldso libdl
endif

include $(RGMK)
