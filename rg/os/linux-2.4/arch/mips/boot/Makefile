#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 1995, 1998, 2001 by Ralf Baechle
#

TOPDIR=../../..
RGSRC=$(TOPDIR)/../..
USE_STANDARD_AS_RULE := true
include $(RGSRC)/envir.mak

#
# Some DECstations need all possible sections of an ECOFF executable
#
ifdef CONFIG_DECSTATION
  E2EFLAGS = -a
else
  E2EFLAGS =
endif

#
# Drop some uninteresting sections in the kernel.
# This is only relevant for ELF kernels but doesn't hurt a.out
#
drop-sections	= .reginfo .mdebug
strip-flags	= $(addprefix --remove-section=,$(drop-sections))

ifeq ($(CONFIG_RG_BZIP2_COMPRESSED_KERNEL),y)
COMPRESS	= bzip2 -c9
endif
ifeq ($(CONFIG_RG_GZIP_COMPRESSED_KERNEL),y)
COMPRESS	= gzip -c9
endif
ifeq ($(CONFIG_RG_LZMA_COMPRESSED_KERNEL),y)
COMPRESS	= $(BUILDDIR)/pkg/lzma/SRC/lzma e -so
endif

all: vmlinux.ecoff addinitrd

vmlinux.ecoff:	$(CONFIGURE) elf2ecoff $(TOPDIR)/vmlinux
	./elf2ecoff $(TOPDIR)/vmlinux vmlinux.ecoff $(E2EFLAGS)

elf2ecoff: elf2ecoff.c
	$(HOSTCC) -o $@ $^

addinitrd: addinitrd.c
	$(HOSTCC) -o $@ $^

addheader: addheader.c
	$(HOSTCC) -o $@ $^

vmlinux.bin: $(PROD_KERNEL_IMAGE)
	$(OBJCOPY) -O binary -R .note -R .comment -R .mdebug -S $^ $@

vmlinuz.bin.gz: vmlinux.bin
	$(COMPRESS) $^ > $@
	
# Don't build dependencies, this may die if $(CC) isn't gcc
dep:

clean:
	rm -f vmlinux.bin $(PROD_LINUX_TARGET) vmlinuz.bin.gz
	rm -f vmlinux.ecoff
	rm -f zImage zImage.tmp

mrproper:
	rm -f vmlinux.bin $(PROD_LINUX_TARGET) vmlinuz.bin.gz
	rm -f vmlinux.ecoff
	rm -f addinitrd
	rm -f elf2ecoff

dummy:

include $(TOPDIR)/Rules.make
