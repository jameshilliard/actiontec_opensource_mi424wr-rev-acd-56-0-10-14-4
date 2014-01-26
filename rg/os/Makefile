RGSRC= ../
include $(RGSRC)/envir.mak

ifndef CONFIG_RG_OS
ifdef IS_BUILDDIR
  $(error CONFIG_RG_OS not defined!)
endif
endif

ifdef CONFIG_RG_OS_LINUX
  ifeq ($(CONFIG_RG_OS),LINUX_24)
    ARCHCONFIG_FIRST_TASKS+=cp_kernel
  endif
endif

ARCHCONFIG_FIRST_TASKS+=create_links
LINUX_AUTOCONF_H=$(KERNEL_DIR)/$(AUTOCONF_H)
ifdef CONFIG_RG_OS_LINUX
  ARCHCONFIG_FIRST_TASKS+=$(LINUX_AUTOCONF_H)
endif

# Override the define in envir.mak We need the link NOT the real dir.
KERNEL_DIR=$(CURDIR)/kernel

ifndef CONFIG_RG_JPKG
  SUBDIRS+=$(notdir $(KERNEL_DIR))
else
  ifdef DOING_MAKE_CONFIG
    SUBDIRS+=$(notdir $(KERNEL_DIR))
  endif
endif

CLEAN+=$(wildcard $(STATIC_MOD_DIR)/*) $(BUILDDIR)/$(notdir $(OPENRG_IMG)) $(BUILDDIR)/$(notdir $(OPENRG_PROD)) layout.bin layout.sec

ifndef CONFIG_RG_JPKG
  OTHER_TASKS+=$(OPENRG_IMG) $(BUILDDIR)/$(notdir $(OPENRG_IMG))
endif

ifdef CONFIG_RG_GPL
  $(shell touch $(BUILDDIR)/os/linux/arch/$(ARCH)/ramdisk/mod.img)
  $(shell touch $(BUILDDIR)/os/linux/arch/$(ARCH)/ramdisk/ramdisk.gz)
  $(shell touch $(BUILDDIR)/os/linux/arch/$(ARCH)/cramdisk/cramfs.img)
  $(shell touch $(BUILDDIR)/os/linux/arch/$(ARCH)/cramdisk/cramfs_init.img)
  $(shell touch $(BUILDDIR)/os/linux/needed_symbols.lst)
OTHER_TASKS+=$(OPENRG_IMG)
else
  ifeq ($(CONFIG_RG_PROD_IMG),y)
    OTHER_TASKS+=$(OPENRG_PROD) $(BUILDDIR)/$(notdir $(OPENRG_PROD))
  else  
    OTHER_TASKS+=$(if $(CONFIG_RG_JPKG),,image_size_verify)
  endif
  
  ifndef CONFIG_RG_JPKG
    OTHER_TASKS+=end_message
  endif
endif

ifdef CONFIG_RG_OS_LINUX_24
  JPKG_EXPORTED_DIR+=linux-2.4
  # .proc and .arch are files that are created in os/linux-2.4/include/asm/ 
  # and should not be exported as src.  
  DONT_EXPORT+=.proc .arch
  CD_EXPORTED_FILES+=Rules_common.make Makefile.vmlinuz
endif

ifeq ($(CONFIG_RG_OS),ECOS)
  SUBDIRS=ecos
  ARCHCONFIG_SUBDIRS=$(SUBDIRS)
endif

ifdef CONFIG_RG_FACTORY_SETTINGS
  FACTORY_SETTINGS=$(BUILDDIR)/pkg/main/rg_factory
endif

ifndef CONFIG_RG_JPKG
ifneq ($(CONFIG_RG_OS),VXWORKS)
  ifneq ($(MAKECMDGOALS),archconfig)
# Adding explicit all: I have to include the boot Makefile before rg.mak for
# distribution purposes but I want the all: the be the first rule. (Ugly? yes,
# working? don't touch)
all:

    include Makefile.vmlinuz
  endif
endif
endif

include $(RGMK)

$(OPENRG_IMG): dummy

end_message: dummy
	@echo
	@echo "OpenRG image is ready at $(OPENRG_IMG)"
ifneq ($(CONFIG_RG_UML),)
	@echo "To run this image: "
	@echo "$$ jrguml burn    # 'burn' (load) the new image"
	@echo "$$ jnetwork start # connect uml0 (WAN) and uml1 (LAN) interfaces to UML"
	@echo "$$ jrguml start --std # start running OpenRG/UML"
	@echo "To later stop OpenRG/UML, run 'jrguml stop' from a DIFFERENT terminal"
	@echo "For further assistance see pkg/tools/README"
endif

$(BUILDDIR)/$(notdir $(OPENRG_IMG)): $(notdir $(OPENRG_IMG))
	@$(RG_LN) $(OPENRG_IMG) $@

ifeq ($(CONFIG_RG_PROD_IMG),y)
  ifeq ($(findstring $(DIST), RGLOADER_RTA770W RGLOADER_WADB100G), $(DIST))
    # Create RGLoader image: 'jump boot' and 'boot' sections only
    BOOTSTRAP_CMD_PARAM=-s '0 ../vendor/broadcom/bcm963xx/boot/jmp_boot.img'
    BOOTSTRAP=$(OPENRG_IMG)
    # No factory settings or OpenRG image when creating RGLoader
    FACTORY_SETTINGS=
    PROD_SKIP_IMAGE=y
  endif
  ifdef BOOTSTRAP
    BOOTSTRAP_CMD_PARAM+=-b $(BOOTSTRAP)
  endif
  ifdef FACTORY_SETTINGS
    FACTORY_SETTINGS_CMD_PARAM=-f $(FACTORY_SETTINGS)
  endif
  ifneq ($(PROD_SKIP_IMAGE),y)
    IMAGE_PARAM=-i $(OPENRG_IMG)
  endif
$(OPENRG_PROD): $(OPENRG_IMG) $(BOOTSTRAP) $(FACTORY_SETTINGS)
	$(BUILDDIR)/pkg/perm_storage/build_flash -v $(BOOTSTRAP_CMD_PARAM) \
	  $(FACTORY_SETTINGS_CMD_PARAM) $(FLASH_LAYOUT_CMD_PARAM) \
	  $(IMAGE_PARAM) -o $@ -p $(FLASH_IMG_OFFSET)

$(BUILDDIR)/$(notdir $(OPENRG_PROD)): $(notdir $(OPENRG_PROD))
	@$(RG_LN) $(OPENRG_PROD) $@
else	
image_size_verify: $(OPENRG_IMG)
	$(BUILDDIR)/pkg/perm_storage/build_flash
	@if ((\
	    $(shell du -b $(OPENRG_IMG) | awk '{print $$1}')\
	    >\
	    $(shell $(BUILDDIR)/pkg/perm_storage/build_flash -v | grep IMAGE | head -n 1 | awk '{print $$8}')\
	    )); \
	then echo Error: Image too big && exit 1; \
	fi
endif

modules:
	make -C $(KERNEL_DIR) $@

cp_kernel:
	$(MKDIR) $(BUILDDIR)/os/
	$(RG_CP_LN) $(RGSRC)/os/linux-2.4 $(BUILDDIR)/os/
	ln -sfn $(RGSRC)/os/Makefile.vmlinuz $(BUILDDIR)/os
	ln -sfn $(RGSRC)/os/Makefile $(BUILDDIR)/os
	ln -sfn $(RGSRC)/os/Rules_common.make $(BUILDDIR)/os

create_links:
ifdef CONFIG_RG_OS_LINUX
  ifeq ($(CONFIG_RG_OS),LINUX_22)
	$(RG_LN) $(RGSRC)/linux $(CURDIR)/linux-2.2
	$(RG_LN) $(CURDIR)/linux-2.2 $(CURDIR)/linux
  endif
  ifeq ($(CONFIG_RG_OS),LINUX_24)
	$(RG_LN) $(CURDIR)/linux-2.4 $(CURDIR)/linux
  endif
	@$(RG_LN) $(CURDIR)/linux $(KERNEL_DIR)
	@$(MKDIR) $(STATIC_MOD_DIR)
else
ifeq ($(CONFIG_RG_OS),VXWORKS)
	@$(RG_LN) $(PWD_SRC)/vx $(KERNEL_DIR)
endif
endif

$(LINUX_AUTOCONF_H):
	@cat $(RG_CONFIG_H) | grep -v "extern " > $@

ifdef CONFIG_RG_OS_LINUX
del_subdirs:
	rm -rf $(STATIC_MOD_DIR)
endif

.PHONY: cp_kernel create_links modules del_subdirs
