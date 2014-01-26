TOPDIR=../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

MSRC=wchar.c
MOBJ=btowc.o wctob.o mbsinit.o mbrlen.o mbrtowc.o wcrtomb.o mbsrtowcs.o \
  wcsrtombs.o _wchar_utf8sntowcs.o _wchar_wcsntoutf8s.o \
  __mbsnrtowcs.o __wcsnrtombs.o wcwidth.o wcswidth.o

ifeq ($(strip $(UCLIBC_HAS_LOCALE)),y)
  MOBJ+=iconv.o
endif

MSRC2=wstdio.c
MOBJ2=fwide.o fgetwc.o getwchar.o fgetws.o fputwc.o putwchar.o fputws.o \
  ungetwc.o

# getwc (fgetwc alias) getwc_unlocked (fgetwc_unlocked alias)
# putwc (fputwc alias) putwc_unlocked (fputwc_unlocked alias)

# fwscanf  wscanf  swscanf  vfwscanf  vwscanf  vswscanf
# wcsftime

CFLAGS+=-DL_$*

OTHER_OBJS=$(MOBJ) $(MOBJ2)
O_OBJS_$(LIBC)+=$(OTHER_OBJS)

include $(RGMK)

$(MOBJ): $(MSRC)
$(MOBJ2): $(MSRC2)

iconv.target: wchar.c
	$(TARGET_CC) $(CFLAGS) -DL_iconv_main wchar.c -o $@
	$(STRIPTOOL) -x -R .note -R .comment $@

