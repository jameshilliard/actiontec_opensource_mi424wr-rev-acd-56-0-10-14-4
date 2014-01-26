RGSRC=../../../
include $(RGSRC)/envir.mak

INTERNAL_HEADERS+=image.h
MKIMAGE=mkimage

LOCAL_TARGET=$(MKIMAGE)
O_OBJS_$(LOCAL_TARGET)=mkimage.o crc32.o

CD_EXPORTED_FILES+=u-boot.README u-boot.patch

VMLINUX=$(BUILDDIR)/os/linux/vmlinux
VMLINUX_BIN=vmlinux_bin
SYSTEM_MAP=$(BUILDDIR)/os/linux/System.map
GET_SYMBOL=$(shell grep -w $1 $(SYSTEM_MAP) | awk '{ printf "0x%s", $$1 }')

GZIP=gzip -f -9
BZIP2=bzip2
LZMA=lzma -z -9

ifeq ($(ARCH),ppc)
  LOADADDR=0x00000000
  KERNEL_ENTRY=0x00000000
endif
ifeq ($(ARCH),mips)
  LOADADDR=$(call GET_SYMBOL, _ftext)
  KERNEL_ENTRY=$(call GET_SYMBOL, kernel_entry)
endif

ifeq ($(UBOOT_COMP),)
  UBOOT_COMP=lzma
endif
ifeq ($(UBOOT_COMP),none)
  ZIMAGE=$(VMLINUX_BIN)
endif
ifeq ($(UBOOT_COMP),gzip)
  ZIMAGE=$(VMLINUX_BIN).gz
endif
ifeq ($(UBOOT_COMP),bzip2)
  ZIMAGE=$(VMLINUX_BIN).bz2
endif
ifeq ($(UBOOT_COMP),lzma)
  ZIMAGE=$(VMLINUX_BIN).lzma
endif


LOCAL_TARGET=$(MKIMAGE)
O_OBJS_$(MKIMAGE)=mkimage.o crc32.o

include $(RGMK)

ifdef IS_BUILDDIR

%.gz: %
	$(GZIP) $^

%.bz2: %
	$(BZIP2) $^

%.lzma: %
	$(LZMA) $^

$(VMLINUX_BIN): $(VMLINUX)
	$(OBJCOPY) -O binary $^ $@

$(OPENRG_IMG): $(MKIMAGE) $(ZIMAGE)
	$(MKIMAGE) -n 'OpenRG' -A $(ARCH) -O linux -T kernel -C $(UBOOT_COMP) \
	-a $(LOADADDR) -e $(KERNEL_ENTRY) -d $(ZIMAGE) $@
	$(RM) $(ZIMAGE)

endif
