ifeq ($(DEBUG_FLAG),1)
DEBUG_MAKE=debug
endif

# Automatic dependency.
CFLAGS+=$(if $(filter %.o,$@),-MMD $(if $(findstring 2.95,$(GCC_VERSION)),,-MP -MF $(@:%.o=%.d) -MT $@))
MOD_CFLAGS+=$(if $(filter %.o,$@),-MMD $(if $(findstring 2.95,$(GCC_VERSION)),,-MP -MF $(@:%.o=%.d) -MT $@))
LOCAL_CFLAGS+=$(if $(filter %.o,$@),-MMD -MP -MF $(@:%.o=%.d) -MT $@)
-include *.d

# Generic rule for recursive subdirs:
# (The code is here and not in rg.mak because it is used by linux kernel in
# ramdisk and CD creation)
# Create a variable that contains all subdirs you want using this pattern:
# DIR/ACTION.subdir__ where DIR is the dir name, action is the action name 
# (like clean) and .subdir__ is a fix sufix. Add a depandancy to it.
# This will make -C DIR ACTION.
BUILD_SUB_DIR=$(if $(filter /%,$(dir $@)),,$(CURDIR)/)$(dir $@)
MKDIR_SUBDIR=$(MKDIR) $(BUILD_SUB_DIR)
ifdef CONFIG_RG_JPKG
  MKDIR_SUBDIR+= $(BUILD_SUB_DIR:$(PWD_BUILD)%=$(PWD_JPKG)%)
endif

BUILD_SUB_DIR_REL=$(subst $(if $(findstring $(BUILDDIR),$(BUILD_SUB_DIR)),$(BUILDDIR)/,$(RGSRC)/),,$(BUILD_SUB_DIR))

THE_MAKEFILE=$(if $(wildcard $(BUILD_SUB_DIR)/Makefile),$(BUILD_SUB_DIR)/Makefile,$(PWD_SRC)/$(dir $@)/Makefile)

IS_JPKG_SUBDIR=$(if $(DOING_MAKE_CONFIG),,$(if $(CONFIG_RG_JPKG),$(if $(filter $(JPKG_EXPORTED_DIR),$(@D)),y)))
# if "make config" and the subdir is in the list of LINK_DIRS then
# create the links
LINK_THE_DIR=$(if $(DOING_MAKE_CONFIG),\
  $(if $(filter $(@D),$(LINK_DIRS)), $(MKDIR) \
  $(dir $(BUILD_SUB_DIR)) && $(RG_CP_LN) \
  $(BUILD_SUB_DIR:$(BUILDDIR)%=$(RGSRC)%) `dirname $(BUILD_SUB_DIR)` && ))

%.subdir__:
ifdef IS_BUILDDIR
ifdef MAKEFILE_DEBUG
	@echo "PWD_JPKG:$(PWD_JPKG)"
	@echo "CURDIR:$(CURDIR)"
	@echo "BUILD_SUB_DIR:$(BUILD_SUB_DIR)"
	@echo "IS_JPKG_SUBDIR:$(if $(IS_JPKG_SUBDIR),YES,NO)"
endif
	$(if $(filter _DUMMY_SUBDIR_%,$@), \
	  , \
	    $(if $(IS_JPKG_SUBDIR), \
	      , \
	        $(if $(wildcard $(THE_MAKEFILE)), \
	         $(LINK_THE_DIR) $(MKDIR_SUBDIR) && $(MAKE) -f $(THE_MAKEFILE) \
	         $(DEBUG_MAKE) -C $(BUILD_SUB_DIR) $(patsubst %.subdir__,%,$(notdir $@)), \
  	         $(if $(CONFIG_RG_GPL), \
	            , \
	            $(error missing Makefile $(THE_MAKEFILE)) \
	         ) \
	       ) \
	    ) \
	 )
else
	@echo "subdir: $(dir $@)"
endif

_DUMMY_SUBDIR_:

# RAMDISK (ramdisk and cramfs) logic
ifndef RAMDISK_SUBDIRS
  RAMDISK_SUBDIRS = $(SUBDIRS)
endif
RAMDISK_SUBDIRS_ = $(RAMDISK_SUBDIRS:%=%/ramdisk.subdir__)

# TODO: I add a suffix/prefix to RAMDISK_FILES so I can make a rule from
# it without clashing with the filename. It can (should?) be changed to
# be target_file: src_file
ALL_RAMDISK_FILES:=$(RAMDISK_FILES) $(RAMDISK_MODULES_FILES) \
  $(RAMDISK_LIB_FILES) $(RAMDISK_ETC_FILES) $(RAMDISK_VAR_FILES) \
  $(RAMDISK_BIN_FILES) $(BOOTLDR_BIN_FILES)
RAMDISK_BIN_FILES+=$(filter-out $(NOT_FOR_RAMDISK) $(notdir $(ALL_RAMDISK_FILES)),$(TARGET))
RAMDISK_LIB_FILES+=$(filter-out $(NOT_FOR_RAMDISK) $(notdir $(ALL_RAMDISK_FILES)),$(addprefix $(SO_SUBDIR)/,$(notdir $(SO_TARGET))) $(addprefix modules/,$(filter-out $(MOD_2_STAT) $(NOT_FOR_RAMDISK) $(MOD_O_TARGET),$(RAMDISK_MODULES_PERMANENT_FILES))))
RAMDISK_MODULES_FILES+=$(filter-out $(NOT_FOR_RAMDISK) $(notdir $(ALL_RAMDISK_FILES)),$(filter-out $(MOD_2_STAT) $(NOT_FOR_RAMDISK) $(MOD_O_TARGET) $(RAMDISK_MODULES_PERMANENT_FILES),$(MOD_TARGET)))
ifdef BOOTLDR_RAMDISK
  RAMDISK_FILES=
  RAMDISK_BIN_FILES=$(BOOTLDR_BIN_FILES)
  RAMDISK_MODULES_FILES=$(BOOTLDR_MODULES)
endif

RAMDISK_FILES_=$(sort $(RAMDISK_FILES:%=__%_rd) \
	       $(RAMDISK_LIB_FILES:%=__/lib/%_rd) \
	       $(RAMDISK_ETC_FILES:%=__/etc/%_rd) \
	       $(RAMDISK_BIN_FILES:%=__/bin/%_rd) \
	       $(RAMDISK_VAR_FILES:%=__/var/%_rd))

RAMDISK_MODULES_FILES_=$(RAMDISK_MODULES_FILES:%=__/lib/modules/%_rd)

$(RAMDISK_FILES_):
	@$(call RAMDISK_CP_RO_FUNC,$(notdir $(@:__%_rd=%)),$(@:__%_rd=%),$(if $(filter /bin/%,$(@:__%_rd=%)),$(if $(CONFIG_BINFMT_FLAT),DONT_STRIP,STRIP),$(if $(filter /lib/%,$(@:__%_rd=%))$(filter /etc/%.so,$(@:__%_rd=%)),STRIP_DEBUG_SYM,DONT_STRIP)))

$(RAMDISK_MODULES_FILES_):
	@$(call $(if $(CONFIG_RG_MODFS),RAMDISK_CP_MOD_FUNC,RAMDISK_CP_RO_FUNC),$(notdir $(@:__%_rd=%)),$(@:__%_rd=%),STRIP_DEBUG_SYM)
	
ramdisk:: $(RAMDISK_FIRST_TASKS) $(RAMDISK_FILES_) $(RAMDISK_MODULES_FILES_) $(RAMDISK_SUBDIRS_) $(RAMDISK_LAST_TASKS)

# If the executable is not already stripped, strip it
ifndef CONFIG_BINFMT_FLAT
  ramdisk:: $(RAMDISK_STRIPPED_FILES_)
endif

dummy:

.PHONY: dummy ramdisk
