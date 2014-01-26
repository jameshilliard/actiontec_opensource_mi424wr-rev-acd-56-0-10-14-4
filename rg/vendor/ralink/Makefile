RGSRC=../..
include $(RGSRC)/envir.mak

SUBDIRS+=$(if $(CONFIG_RALINK_RT2560), rt2560 8021X)
SUBDIRS+=$(if $(CONFIG_RALINK_RT2561), rt2561/8021X rt2561/Module)

include $(RGMK)
