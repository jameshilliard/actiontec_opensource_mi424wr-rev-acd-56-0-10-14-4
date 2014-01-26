TOPDIR=../../../
include $(TOPDIR)Rules.mak

A_TARGET=$(LIBC)

# calloc.c can be found at uClibc/libc/stdlib/calloc.c 
O_OBJS_$(LIBC)=malloc.o free.o realloc.o memalign.o heap_alloc.o \
  heap_alloc_at.o heap_free.o

# Turn on malloc debugging if requested
ifeq ($(strip $(UCLIBC_MALLOC_DEBUGGING)),y)
  O_OBJS_$(LIBC)+=malloc_debug.o heap_debug.o
  CFLAGS+=-DMALLOC_DEBUGGING -DHEAP_DEBUGGING
  ifeq ($(strip $(UCLIBC_UCLINUX_BROKEN_MUNMAP)),y)
    CFLAGS+=-DMALLOC_MMB_DEBUGGING
  endif
endif

include $(RGMK)

