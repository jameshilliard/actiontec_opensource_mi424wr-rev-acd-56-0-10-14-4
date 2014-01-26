RGSRC=../../
include $(RGSRC)/envir.mak

SUBDIRS+=$(if $(CONFIG_RG_BZIP2),bzip2)
SUBDIRS+=$(if $(CONFIG_BOOTLDR_UBOOT),u-boot)

include $(RGMK)
