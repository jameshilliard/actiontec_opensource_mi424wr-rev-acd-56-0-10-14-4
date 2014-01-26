TOPDIR=../../
include $(TOPDIR)Rules.mak

SUBDIRS=assert ctype dirent file fnmatch glob internals mntent syslog time \
  utmp locale sysvipc statfs error ttyent gnu search
       
ifeq ($(strip $(UCLIBC_HAS_REGEX)),y)
  SUBDIRS+=regex
endif

ifeq ($(strip $(UCLIBC_HAS_THREADS)),y)
  SUBDIRS+=pthread
endif

ifeq ($(strip $(UCLIBC_HAS_WCHAR)),y)
  SUBDIRS+=wctype wchar
endif

ifeq ($(strip $(UCLIBC_HAS_FTW)),y)
  SUBDIRS+=ftw
endif

include $(RGMK)

