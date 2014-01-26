RGSRC=../../../..
include $(RGSRC)/envir.mak

# ri408wr.o - generic board support (must be statically linked to kernel)
MOD_TARGET=ri408wr.o
MOD_2_STAT=$(MOD_TARGET)
O_OBJS_ri408wr.o+=arch.o
O_OBJS_ri408wr.o+=$(if $(CONFIG_PCI), pci.o)

EXPORT_HEADERS_DIR=vendor
EXPORT_HEADERS+=plf_leds.c__c/plf_leds.c
ifndef CONFIG_RG_GPL
  EXPORT_HEADERS+=plf_leds.h
endif

include $(RGMK)
