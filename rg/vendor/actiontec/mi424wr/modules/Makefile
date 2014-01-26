RGSRC=../../../..
include $(RGSRC)/envir.mak

# mi424wr.o - generic board support (must be statically linked to kernel)
MOD_TARGET=mi424wr.o
MOD_2_STAT=$(MOD_TARGET)

O_OBJS_mi424wr.o+=latch.o

ifdef CONFIG_PCI
  O_OBJS_mi424wr.o+=pci.o
endif

EXPORT_HEADERS_DIR=vendor
EXPORT_HEADERS+=latch.h plf_leds.c__c/plf_leds.c

ifndef CONFIG_RG_GPL
  EXPORT_HEADERS+=plf_leds.h
endif

include $(RGMK)
