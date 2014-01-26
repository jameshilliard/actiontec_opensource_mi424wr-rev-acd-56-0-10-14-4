RGSRC=../../
include $(RGSRC)/envir.mak

SUBDIRS=pppd kernel
ifndef CONFIG_RG_GPL
A_TARGET=libppp.a
L_OBJS_$(A_TARGET)+=pppd/ppptask.o
L_OBJS_$(A_TARGET)+=pppd/plugins/plugins.o

ifneq ($(CONFIG_RG_PPTPS),)
  SUBDIRS+=pptp-server
  L_OBJS_$(A_TARGET)+=pptp-server/pptps_task.o
endif

ifdef CONFIG_RG_PPPOES
  SUBDIRS+=pppoes
  L_OBJS_$(A_TARGET)+=pppoes/pppoes_task.o
endif

ifdef CONFIG_RG_PPPOS
  SUBDIRS+=pppos
  L_OBJS_$(A_TARGET)+=pppos/pppos_task.o
endif

SUBDIRS+=mgt main

ifneq ($(CONFIG_RG_WBM),)
  SUBDIRS+=wbm
  L_OBJS_$(A_TARGET)+=wbm/wbm.o
endif

L_OBJS_$(A_TARGET)+=main/ppp_main.o
L_OBJS_$(A_TARGET)+=mgt/ppp_dev.o

endif #CONFIG_RG_GPL

include $(RGMK)
