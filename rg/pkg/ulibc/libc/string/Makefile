TOPDIR=../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

MSRC=wstring.c
MOBJ=basename.o bcopy.o bzero.o dirname.o ffs.o memccpy.o memchr.o memcmp.o \
  memcpy.o memmove.o mempcpy.o memrchr.o memset.o rawmemchr.o stpcpy.o \
  stpncpy.o strcasecmp.o strcasestr.o strcat.o strchrnul.o strchr.o \
  strcmp.o strcpy.o strcspn.o strdup.o strlen.o strncasecmp.o strncat.o \
  strncmp.o strncpy.o strndup.o strnlen.o strpbrk.o strrchr.o strsep.o \
  strspn.o strstr.o strtok.o strtok_r.o strerror.o _susv3_strerror_r.o \
  _string_syserrmsgs.o _glibc_strerror_r.o \
  _string_syssigmsgs.o sys_siglist.o strsignal.o psignal.o \
  __xpg_basename.o strlcat.o strlcpy.o sys_errlist.o memmem.o

ifeq ($(strip $(UCLIBC_HAS_WCHAR)),y)
  WMOBJ+=wcscasecmp.o wcscat.o wcschrnul.o wcschr.o wcscmp.o wcscpy.o wcscspn.o \
    wcsdup.o wcslen.o wcsncasecmp.o wcsncat.o wcsncmp.o wcsncpy.o \
    wcsnlen.o wcspbrk.o wcsrchr.o wcsspn.o wcsstr.o wcstok.o wmemchr.o \
    wmemcmp.o wmemcpy.o wmemmove.o wmempcpy.o wmemset.o wcpcpy.o wcpncpy.o \
    __wcslcpy.o wcsxfrm.o strxfrm.o # wcscoll strcoll.o
  MOBJ+=$(WMOBJ)

  # For each object from WMOBJ list add CFLAGS_<object name>+=-DWANT_WIDE
  COMMAND_FILE=variable.mak
  FOR_EACH=$(WMOBJ)
  INDEX_VARIABLE=CFLAGS_$(INDEX)
  INDEX_VAL=-DWANT_WIDE
  include $(LOOP)
endif

CFLAGS+=-DL_$*

OTHER_OBJS=$(MOBJ)
O_OBJS_$(LIBC)+=$(OTHER_OBJS)

include $(RGMK)

$(MOBJ): $(MSRC)
