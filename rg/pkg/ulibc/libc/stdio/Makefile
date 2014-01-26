TOPDIR=../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

# Note: The *64.o objects are empty when compiled without large file support.
#

# Note: Use the libpthreads of: flockfile.o ftrylockfile.o funlockfile.o
#       Also, maybe move __fsetlocking.o as well?

MSRC=stdio.c
MOBJ=fclose.o fflush.o fopen.o freopen.o perror.o remove.o \
  setbuf.o setvbuf.o fgetc.o fgets.o fputc.o fputs.o \
  getc.o getchar.o gets.o putc.o putchar.o puts.o \
  ungetc.o fread.o fwrite.o fgetpos.o fseek.o fsetpos.o ftell.o \
  rewind.o clearerr.o feof.o ferror.o \
  fileno.o fdopen.o getw.o putw.o setbuffer.o setlinebuf.o fcloseall.o \
  fopen64.o freopen64.o ftello64.o fseeko64.o fsetpos64.o fgetpos64.o \
  __fbufsize.o __freading.o __fwriting.o __freadable.o __fwritable.o \
  __flbf.o __fpurge.o __fpending.o _flushlbf.o \
  fopencookie.o fmemopen.o open_memstream.o \
  __fsetlocking.o flockfile.o ftrylockfile.o funlockfile.o \
  _stdio_fopen.o _stdio_fread.o _stdio_fwrite.o _stdio_adjpos.o \
  _stdio_lseek.o _stdio_init.o \
  _stdio_fsfopen.o _stdio_fdout.o _uintmaxtostr.o _stdio_strerror_r.o \
  getdelim.o getline.o ctermid.o

MSRC2=printf.c
MOBJ2=vsnprintf.o vdprintf.o vasprintf.o vprintf.o vsprintf.o \
  fprintf.o snprintf.o dprintf.o asprintf.o printf.o sprintf.o

ifeq ($(strip $(UCLIBC_HAS_WCHAR)),y)
  MOBJ+=_wstdio_fwrite.o
  MOBJ2+=fwprintf.o wprintf.o swprintf.o vwprintf.o vswprintf.o vfwprintf.o
endif

ifneq ($(strip $(USE_OLD_VFPRINTF)),y)
  MOBJ2+=_ppfs_init.o _ppfs_prepargs.o _ppfs_setargs.o _ppfs_parsespec.o \
    vfprintf.o _store_inttype.o _load_inttype.o register_printf_function.o \
    parse_printf_format.o
endif
# _do_one_spec.o 

ifeq ($(strip $(UCLIBC_HAS_FLOATS)),y)
  MOBJ2+=_dtostr.o
endif

MSRC3=scanf.c
MOBJ3=scanf.o sscanf.o fscanf.o vscanf.o vsscanf.o vfscanf.o

O_OBJS_$(LIBC)=popen.o tmpfile.o tmpnam.o tmpnam_r.o tempnam.o
ifeq ($(strip $(USE_OLD_VFPRINTF)),y)
  O_OBJS_$(LIBC)+=old_vfprintf.o
endif

CFLAGS+=-DL_$*

OTHER_OBJS=$(MOBJ) $(MOBJ2) $(MOBJ3)
O_OBJS_$(LIBC)+=$(OTHER_OBJS)

include $(RGMK)

$(MOBJ): $(MSRC)
$(MOBJ2): $(MSRC2)
$(MOBJ3): $(MSRC3)

