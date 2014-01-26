#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 1994, 1995, 1996 by Ralf Baechle
# DECStation modifications by Paul M. Antoine, 1996
# Copyright (C) 2002  Maciej W. Rozycki
#
# This file is included by the global makefile so that you can add your own
# architecture-specific flags and dependencies. Remember to do have actions
# for "archclean" and "archdep" for cleaning up and making dependencies for
# this architecture
#

#
# Select the object file format to substitute into the linker script.
#
ifdef CONFIG_CPU_LITTLE_ENDIAN
tool-prefix	= mipsel-linux-
ld-emul		= elf32ltsmip
else
tool-prefix	= mips-linux-
ld-emul		= elf32btsmip
endif

ifdef CONFIG_CROSSCOMPILE
CROSS_COMPILE	= $(tool-prefix)
endif

check_gcc = $(shell if $(CC) $(1) -S -o /dev/null -xc /dev/null > /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi)

#
#
# GCC uses -G 0 -mabicalls -fpic as default.  We don't want PIC in the kernel
# code since it only slows down the whole thing.  At some point we might make
# use of global pointer optimizations but their use of $28 conflicts with
# the current pointer optimization.
#
# The DECStation requires an ECOFF kernel for remote booting, other MIPS
# machines may also.  Since BFD is incredibly buggy with respect to
# crossformat linking we rely on the elf2ecoff tool for format conversion.
#
GCCFLAGS	:= -I$(TOPDIR)/include/asm/gcc
GCCFLAGS	+= -G0 -mno-abicalls -fno-pic -pipe
GCCFLAGS	+= $(call check_gcc, -mabi=32,)
LINKFLAGS	+= -G0 -static # -N
MODFLAGS	+= -mlong-calls

ifdef CONFIG_KGDB
GCCFLAGS	+= -g
ifdef CONFIG_SB1XXX_CORELIS
GCCFLAGS	+= -mno-sched-prolog -fno-omit-frame-pointer
endif
endif

GCCFLAGS	+= $(ARCH_LINUX_FLAGS)

AFLAGS		+= $(GCCFLAGS)
CFLAGS		+= $(GCCFLAGS)
LDFLAGS		+= -m $(ld-emul)
ifdef CONFIG_RG_FOOTPRINT_REDUCTION
CFLAGS		+=-ffunction-sections -fdata-sections
LINKFLAGS	+=--gc-sections
endif


#
# We unconditionally build the math emulator
#
CORE_FILES	+= arch/mips/math-emu/fpu_emulator.o
SUBDIRS		+= arch/mips/math-emu

#
# ramdisk/initrd support
# You need a compressed ramdisk image, named ramdisk.gz in
# arch/mips/ramdisk
#
ifneq ($(CONFIG_EMBEDDED_RAMDISK)$(CONFIG_RG_MODFS),)
  SUBDIRS         += arch/mips/ramdisk
  ifdef CONFIG_EMBEDDED_RAMDISK
    CORE_FILES      += arch/mips/ramdisk/ramdisk.o
  endif
  ifdef CONFIG_RG_MODFS
    CORE_FILES	+= arch/mips/ramdisk/modrd.o
  endif
endif

#
# cramfs on ram support
# You need a compressed cramfs image, named cramfs.img in
# arch/mips/ramdisk
#
ifdef CONFIG_COPY_CRAMFS_TO_RAM
  CORE_FILES      += arch/mips/cramdisk/cramdisk.o
  SUBDIRS         += arch/mips/cramdisk
  ifdef CONFIG_RG_INITFS_CRAMFS
    CORE_FILES	  += arch/mips/cramdisk/cramdisk_init.o
  endif
endif

#
# Board-dependent options and extra files
#

#
# Acer PICA 61, Mips Magnum 4000 and Olivetti M700.
#
ifdef CONFIG_MIPS_JAZZ
CORE_FILES	+= arch/mips/jazz/jazz.o
SUBDIRS		+= arch/mips/jazz arch/mips/arc
LIBS		+= arch/mips/arc/arclib.a
LOADADDR	:= 0x80080000
endif

#
# Au1000 (Alchemy Semi PB1000) eval board
#
ifdef CONFIG_MIPS_PB1000
LIBS		+= arch/mips/au1000/pb1000/pb1000.o \
		   arch/mips/au1000/common/au1000.o
SUBDIRS		+= arch/mips/au1000/pb1000 arch/mips/au1000/common
LOADADDR	:= 0x80100000
endif

#
# Au1100 (Alchemy Semi PB1100) eval board
#
ifdef CONFIG_MIPS_PB1100
LIBS          += arch/mips/au1000/pb1100/pb1100.o \
                 arch/mips/au1000/common/au1000.o
SUBDIRS       += arch/mips/au1000/pb1100 arch/mips/au1000/common
LOADADDR      += 0x80100000
endif

#
# Au1500 (Alchemy Semi PB1500) eval board
#
ifdef CONFIG_MIPS_PB1500
LIBS		+= arch/mips/au1000/pb1500/pb1500.o \
		   arch/mips/au1000/common/au1000.o
SUBDIRS		+= arch/mips/au1000/pb1500 arch/mips/au1000/common
LOADADDR	:= 0x80100000
endif

ifdef CONFIG_PCI
CORE_FILES    += arch/mips/pci/pci-core.o
SUBDIRS       += arch/mips/pci
endif

#
# Algorithmics P4032
#
ifdef CONFIG_ALGOR_P4032
CORE_FILES	+= arch/mips/algor/algor.o
SUBDIRS		+= arch/mips/algor
LOADADDR	:= 0x80000000
endif

#
# Baget/MIPS
#
ifdef CONFIG_BAGET_MIPS
SUBDIRS		+= arch/mips/baget arch/mips/baget/prom
LIBS		+= arch/mips/baget/baget.a arch/mips/baget/prom/bagetlib.a
LOADADDR	:= 0x80001000
endif

#
# Cobalt Server
#
ifdef CONFIG_MIPS_COBALT
SUBDIRS		+= arch/mips/cobalt
CORE_FILES	+= arch/mips/cobalt/cobalt.o
LOADADDR	:= 0x80080000
endif

#
# DECstation family
#
ifdef CONFIG_DECSTATION
CORE_FILES	+= arch/mips/dec/dec.o
SUBDIRS		+= arch/mips/dec arch/mips/dec/prom
LIBS		+= arch/mips/dec/prom/rexlib.a
LOADADDR	:= 0x80040000
endif

#
# Galileo EV64120 Board
#
ifdef CONFIG_MIPS_EV64120
LIBS		+= arch/mips/galileo-boards/ev64120/ev64120.o
SUBDIRS		+= arch/mips/galileo-boards/ev64120
LOADADDR	:= 0x80100000
endif

#
# Galileo EV96100 Board
#
ifdef CONFIG_MIPS_EV96100
LIBS		+= arch/mips/galileo-boards/ev96100/ev96100.o \
		   arch/mips/galileo-boards/generic/galboards.o
SUBDIRS		+= arch/mips/galileo-boards/generic \
		   arch/mips/galileo-boards/ev96100
LOADADDR	:= 0x80100000
endif

#
# Globespan IVR eval board with QED 5231 CPU
#
ifdef CONFIG_MIPS_IVR
LIBS		+= arch/mips/ite-boards/ivr/ivr.o \
		   arch/mips/ite-boards/generic/it8172.o
SUBDIRS		+= arch/mips/ite-boards/generic arch/mips/ite-boards/ivr
LOADADDR	:= 0x80100000
endif

#
# HP LaserJet
#
ifdef CONFIG_HP_LASERJET
SUBDIRS		+= arch/mips/hp-lj
LIBS		+= arch/mips/hp-lj/hp-lj.o
LOADADDR	:= 0x80030000
endif

#
# INCAIP
#
ifdef CONFIG_INCAIP
LOADADDR	:= 0x80002000
endif

#
# ADM5120
#
ifdef CONFIG_MIPS_AM5120
LOADADDR	:= 0x80002000
endif

#
# AMAZON
#
ifdef CONFIG_AMAZON
LOADADDR        := 0x80002000
endif

#
# ITE 8172 eval board with QED 5231 CPU
#
ifdef CONFIG_MIPS_ITE8172
LIBS		+= arch/mips/ite-boards/qed-4n-s01b/ite.o \
		   arch/mips/ite-boards/generic/it8172.o
SUBDIRS		+= arch/mips/ite-boards/generic arch/mips/ite-boards/qed-4n-s01b
LOADADDR	:= 0x80100000
endif

#
# MIPS Atlas board
#
ifdef CONFIG_MIPS_ATLAS
LIBS		+= arch/mips/mips-boards/atlas/atlas.o \
		   arch/mips/mips-boards/generic/mipsboards.o
SUBDIRS		+= arch/mips/mips-boards/generic arch/mips/mips-boards/atlas
LOADADDR	:= 0x80100000
endif

#
# MIPS Malta board
#
ifdef CONFIG_MIPS_MALTA
LIBS		+= arch/mips/mips-boards/malta/malta.o \
		   arch/mips/mips-boards/generic/mipsboards.o
SUBDIRS		+= arch/mips/mips-boards/malta arch/mips/mips-boards/generic
LOADADDR	:= 0x80100000
endif

#
# MIPS SEAD board
#
ifdef CONFIG_MIPS_SEAD
LIBS		+= arch/mips/mips-boards/sead/sead.o \
		   arch/mips/mips-boards/generic/mipsboards.o
SUBDIRS		+= arch/mips/mips-boards/generic arch/mips/mips-boards/sead
LOADADDR	:= 0x80100000
endif

#
# Momentum Ocelot board
#
ifdef CONFIG_MOMENCO_OCELOT
# The Ocelot setup.o must be linked early - it does the ioremap() for the
# mips_io_port_base.
CORE_FILES	+= arch/mips/gt64120/common/gt64120.o \
		   arch/mips/gt64120/momenco_ocelot/momenco_ocelot.o
SUBDIRS		+= arch/mips/gt64120/common arch/mips/gt64120/momenco_ocelot
LOADADDR	:= 0x80100000
endif

#
# Momentum Ocelot-G board
#
ifdef CONFIG_MOMENCO_OCELOT_G
# The Ocelot-G setup.o must be linked early - it does the ioremap() for the
# mips_io_port_base.
CORE_FILES	+= arch/mips/momentum/ocelot_g/ocelot_g.o
SUBDIRS		+= arch/mips/momentum/ocelot_g
LOADADDR	:= 0x80100000
endif

#
# Broadcom BCM93725 variants
#
ifdef CONFIG_BCM93725
LIBS          += arch/mips/brcm_boards/bcm93725/bcm93725.o arch/mips/brcm_boards/generic/brcm.o
SUBDIRS       += arch/mips/brcm_boards/generic arch/mips/brcm_boards/bcm93725
LOADADDR      += 0x80000000
TEXTADDR      += 0x80001000
endif

#
# Broadcom BCM93730 variants
#
ifdef CONFIG_BCM93730
LIBS          += arch/mips/brcm_boards/bcm93730/bcm93730.o
SUBDIRS       += arch/mips/brcm_boards/bcm93730
LOADADDR      += 0x80001000
LINKFLAGS     += -Ttext $(word 1,$(LOADADDR))
endif

#
# Broadcom BCM947XX variants
#
ifdef CONFIG_BCM947XX
CFLAGS	      += -I$(TOPDIR)/include/asm/brcm_boards/
LIBS          += arch/mips/brcm_boards/bcm947xx/bcm947xx.o arch/mips/brcm_boards/generic/brcm.o
SUBDIRS       += arch/mips/brcm_boards/generic arch/mips/brcm_boards/bcm947xx
LOADADDR      += 0x80800000
TEXTADDR      += 0x80010000
endif

#
# Broadcom BCM933XX variants
#
ifdef CONFIG_BCM933XX
LIBS          += arch/mips/brcm_boards/bcm933xx/bcm933xx.a
SUBDIRS       += arch/mips/brcm_boards/bcm933xx
LOADADDR      += 0x80010000
endif

#
# Broadcom BCM933XX variants
#
ifdef CONFIG_BCM963XX
LIBS          += arch/mips/brcm_boards/bcm963xx/bcm963xx.o
SUBDIRS       += arch/mips/brcm_boards/bcm963xx
LOADADDR      += 0x80010000
endif

export LOADADDR TEXTADDR

#
# NEC DDB Vrc-5074
#
ifdef CONFIG_DDB5074
SUBDIRS		+= arch/mips/ddb5xxx/common arch/mips/ddb5xxx/ddb5074
LIBS		+= arch/mips/ddb5xxx/common/ddb5xxx.o arch/mips/ddb5xxx/ddb5074/ddb5074.o
LOADADDR	:= 0x80080000
endif

#
# NEC DDB Vrc-5476
#
ifdef CONFIG_DDB5476
SUBDIRS		+= arch/mips/ddb5xxx/common arch/mips/ddb5xxx/ddb5476
LIBS		+= arch/mips/ddb5xxx/common/ddb5xxx.o \
		   arch/mips/ddb5xxx/ddb5476/ddb5476.o
LOADADDR	:= 0x80080000
endif

#
# NEC DDB Vrc-5477
#
ifdef CONFIG_DDB5477
SUBDIRS		+= arch/mips/ddb5xxx/common arch/mips/ddb5xxx/ddb5477
LIBS		+= arch/mips/ddb5xxx/common/ddb5xxx.o \
		   arch/mips/ddb5xxx/ddb5477/ddb5477.o
LOADADDR	:= 0x80100000
endif

#
# NEC Osprey (vr4181) board
#
ifdef CONFIG_NEC_OSPREY
SUBDIRS		+= arch/mips/vr4181/common arch/mips/vr4181/osprey
LIBS		+= arch/mips/vr4181/common/vr4181.o \
		   arch/mips/vr4181/osprey/osprey.o
LOADADDR	:= 0x80002000
endif

#
# NEC Eagle/Hawk (VR4122/VR4131) board
#
ifdef CONFIG_NEC_EAGLE
SUBDIRS		+= arch/mips/vr41xx/common \
		   arch/mips/vr41xx/nec-eagle
LIBS		+= arch/mips/vr41xx/common/vr41xx.o \
		   arch/mips/vr41xx/nec-eagle/eagle.o
LOADADDR	:= 0x80000000
endif

#
# ZAO Networks Capcella (VR4131)
#
ifdef CONFIG_ZAO_CAPCELLA
SUBDIRS		+= arch/mips/vr41xx/common \
		   arch/mips/vr41xx/zao-capcella
LIBS		+= arch/mips/vr41xx/common/vr41xx.o \
		   arch/mips/vr41xx/zao-capcella/capcella.o
LOADADDR	:= 0x80000000
endif

#
# Philips Nino
#
ifdef CONFIG_NINO
CORE_FILES	+= arch/mips/philips/nino/nino.o
SUBDIRS		+= arch/mips/philips/nino
LOADADDR	:= 0x80000000
endif

#
# SGI IP22 (Indy/Indigo2)
#
ifdef CONFIG_SGI_IP22
CORE_FILES	+= arch/mips/sgi-ip22/ip22-kern.o
LIBS		+= arch/mips/arc/arclib.a
SUBDIRS		+= arch/mips/sgi-ip22 arch/mips/arc
#
# Set LOADADDR to >= 0x88069000 if you want to leave space for symmon,
# 0x88002000 for production kernels.  Note that the value must be
# 8kb aligned or the handling of the current variable will break.
#
LOADADDR	:= 0x88002000
endif

#
# Sibyte SB1250 SOC
#
ifdef CONFIG_SIBYTE_SB1250
# This is a LIB so that it links at the end, and initcalls are later
# the sequence; but it is built as an object so that modules don't get
# removed (as happens, even if they have __initcall/module_init)
LIBS		+= arch/mips/sibyte/sb1250/sb1250.o
SUBDIRS		+= arch/mips/sibyte/sb1250
endif

#
# Sibyte SWARM board
#
ifdef CONFIG_SIBYTE_SWARM
LIBS		+= arch/mips/sibyte/swarm/sbswarm.a
SUBDIRS		+= arch/mips/sibyte/swarm
LOADADDR	:= 0x80100000
endif

#
# Sibyte CFE firmware
#
ifdef CONFIG_SIBYTE_CFE
LIBS		+= arch/mips/sibyte/cfe/cfe.a
SUBDIRS		+= arch/mips/sibyte/cfe
endif

#
# SB1 Cache Error handler
#
ifdef CONFIG_SB1_CACHE_ERROR
LIBS		+= arch/mips/sibyte/sb1/sb1kern.a
SUBDIRS		+= arch/mips/sibyte/sb1
endif

#
# SNI RM200 PCI
#
ifdef CONFIG_SNI_RM200_PCI
CORE_FILES	+= arch/mips/sni/sni.o
SUBDIRS		+= arch/mips/sni arch/mips/arc
LIBS		+= arch/mips/arc/arclib.a
LOADADDR	:= 0x80080000
endif

#
# Toshiba JMR-TX3927 board
#
ifdef CONFIG_TOSHIBA_JMR3927
CORE_FILES	+= arch/mips/jmr3927/rbhma3100/jmr3927.o \
		   arch/mips/jmr3927/common/tx3927.o
SUBDIRS		+= arch/mips/jmr3927/rbhma3100 arch/mips/jmr3927/common
LOADADDR	:= 0x80050000
endif


#
# Choosing incompatible machines durings configuration will result in
# error messages during linking.  Select a default linkscript if
# none has been choosen above.
#
vmlinux: arch/$(ARCH)/ld.script

arch/$(ARCH)/ld.script: arch/$(ARCH)/ld.script.in arch/$(ARCH)/Makefile
	sed -e 's/@@LOADADDR@@/$(LOADADDR)/' <$< >$@
LINKFLAGS	+= -T arch/$(ARCH)/ld.script
ifdef TEXTADDR
LINKFLAGS	+= -Ttext $(word 1,$(TEXTADDR))
endif

HEAD := arch/mips/kernel/head.o arch/mips/kernel/init_task.o

SUBDIRS := $(SUBDIRS) $(addprefix arch/mips/, kernel mm lib)
CORE_FILES := arch/mips/kernel/kernel.o arch/mips/mm/mm.o $(CORE_FILES)
LIBS := arch/mips/lib/lib.a $(LIBS)

ifdef CONFIG_BAGET_MIPS

BAGETBOOT = $(MAKE) -C arch/$(ARCH)/baget

balo: vmlinux
	$(BAGETBOOT) balo

endif

ifdef CONFIG_MIPS_EV64120
GALILEOBOOT = $(MAKE) -C arch/$(ARCH)/galileo-boards/ev64120

gboot: vmlinux
	$(MAKE) -C arch/$(ARCH)/galileo-boards/ev64120/compressed
endif

MAKEBOOT = $(MAKE) -C arch/$(ARCH)/boot

vmlinuz.bin.gz: vmlinux
	@$(MAKEBOOT) $@

vmlinux.ecoff: vmlinux
	@$(MAKEBOOT) $@

archclean:
	@$(MAKEBOOT) clean
	rm -f arch/$(ARCH)/ld.script
	$(MAKE) -C arch/$(ARCH)/tools clean
	$(MAKE) -C arch/mips/baget clean

archmrproper:
	@$(MAKEBOOT) mrproper
	$(MAKE) -C arch/$(ARCH)/tools mrproper

archdep:
	if [ ! -f $(TOPDIR)/include/asm-$(ARCH)/offset.h ]; then \
		touch $(TOPDIR)/include/asm-$(ARCH)/offset.h; \
	fi;
	@$(MAKEBOOT) dep

archconfig:: symlinks
	$(MAKE) -C arch/$(ARCH)/tools
