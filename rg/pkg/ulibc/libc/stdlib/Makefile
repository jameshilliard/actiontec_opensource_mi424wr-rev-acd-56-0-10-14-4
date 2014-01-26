TOPDIR=../../
include $(TOPDIR)Rules.mak

ifeq ($(strip $(MALLOC)),y)
  SUBDIRS+=malloc
endif
ifeq ($(strip $(MALLOC_930716)),y)
  SUBDIRS+=malloc-930716
endif
ifeq ($(strip $(MALLOC_STANDARD)),y)
  SUBDIRS+=malloc-standard
endif

A_TARGET=$(LIBC)

MSRC=stdlib.c
MOBJ=abs.o labs.o atoi.o atol.o strtol.o strtoul.o _stdlib_strto_l.o qsort.o \
     bsearch.o llabs.o atoll.o strtoll.o strtoull.o _stdlib_strto_ll.o
# (aliases) strtoq.o strtouq.o
ifeq ($(strip $(UCLIBC_HAS_FLOATS)),y)
  MOBJ+=atof.o
endif

ifeq ($(strip $(UCLIBC_HAS_WCHAR)),y)
  MOBJ+=mblen.o mbtowc.o wctomb.o mbstowcs.o wcstombs.o _stdlib_mb_cur_max.o \
    _stdlib_wcsto_l.o _stdlib_wcsto_ll.o wcstol.o wcstoul.o wcstoll.o \
    wcstoull.o
# (aliases) wcstoq.o wcstouq.o
# wcstod wcstof wcstold
endif

MSRC2=atexit.c
MOBJ2=atexit.o on_exit.o __exit_handler.o exit.o

O_OBJS_$(LIBC)=abort.o getenv.o mkdtemp.o mktemp.o realpath.o mkstemp.o \
  mkstemp64.o rand.o random.o random_r.o setenv.o system.o div.o ldiv.o \
  getpt.o ptsname.o grantpt.o unlockpt.o gcvt.o drand48-iter.o jrand48.o \
  jrand48_r.o lrand48.o lrand48_r.o mrand48.o mrand48_r.o nrand48.o \
  nrand48_r.o rand_r.o srand48.o srand48_r.o calloc.o valloc.o

ifeq ($(strip $(UCLIBC_HAS_FLOATS)),y)
   O_OBJS_$(LIBC)+=strtod.o strtof.o strtold.o drand48.o drand48_r.o \
     erand48.o erand48_r.o
endif

#XXX to avoid drand48-iter
CFLAGS+=-DL_$(subst -,_,$*)

OTHER_OBJS=$(MOBJ) $(MOBJ2)
O_OBJS_$(LIBC)+=$(OTHER_OBJS)

include $(RGMK)

$(MOBJ): $(MSRC)
$(MOBJ2): $(MSRC2)
