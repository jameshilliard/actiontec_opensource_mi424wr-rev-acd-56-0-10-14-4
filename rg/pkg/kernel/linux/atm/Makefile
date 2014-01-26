RGSRC=../../../..
include $(RGSRC)/envir.mak

ifdef CONFIG_ATM_CLIP
  MOD_TARGET=clip_mod.o
  MOD_2_STAT+=$(MOD_TARGET)
endif

O_OBJS_clip_mod.o=clip.o
CFLAGS_clip.o+=-I$(RGSRC)/pkg

EXPORT_HEADERS+=clip.h

include $(RGMK)
