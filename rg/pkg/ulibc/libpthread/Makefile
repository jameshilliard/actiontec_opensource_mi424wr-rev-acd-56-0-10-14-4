TOPDIR=../
include $(TOPDIR)Rules.mak

SUBDIRS+=linuxthreads
ifeq ($(strip $(PTHREADS_DEBUG_SUPPORT)),y)
  SUBDIRS+=linuxthreads_db
endif

include $(ULIBCMK)

