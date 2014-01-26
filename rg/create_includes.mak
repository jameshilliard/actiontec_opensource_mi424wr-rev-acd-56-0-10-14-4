# Create pkg/build/include subdir. We need this to enable the include/linux 
# and include/asm to be differnt for each tree even though all use 
# /usr/local/openrg/<arch>/include subdir.
# Glibc support is only for i386 for ensure/mpatrol needs. 
create_includes:
	rm -rf $(RG_INCLUDE)
	$(MKDIR) $(RG_INCLUDE)
ifdef CONFIG_ULIBC
# The next line fixes a problem that occurs when running make config without
# doing a distclean
	@rm -f $(BUILDDIR)/pkg/ulibc/include/asm
  ifdef ULIBC_IN_TOOLCHAIN
	@cp -f -R --symbolic-link $(TOOLS_PREFIX)/ulibc/include/* $(RG_INCLUDE)
	@$(RG_LN) $(TOOLS_PREFIX)/ulibc/include/bits $(RG_INCLUDE)/bits
  else
	@cp -f -R --symbolic-link $(RGSRC)/pkg/ulibc/include/* $(RG_INCLUDE)
	@$(RG_LN) $(BUILDDIR)/pkg/ulibc/include/bits $(RG_INCLUDE)/bits
  endif
endif
ifdef CONFIG_GLIBC 
	@cp -f -R --symbolic-link $(TOOLS_PREFIX)/$(TARGET_MACHINE)/include/* \
	    $(RG_INCLUDE)
	@rm -rf $(RG_LIB)
	$(MKDIR) $(RG_LIB)
	@cp -f -R --symbolic-link $(TOOLS_PREFIX)/$(TARGET_MACHINE)/lib/* \
	    $(RG_LIB)
	# Kernel's scsi includes are newer
	@rm -rf $(RG_INCLUDE)/scsi
endif
ifdef CONFIG_RG_OS_LINUX
	rm -rf $(RG_INCLUDE)/linux $(RG_INCLUDE)/asm
	# Kernel's scsi includes are newer
	@rm -rf $(RG_INCLUDE)/scsi
	@$(RG_LN) $(BUILDDIR)/os/kernel/include/linux $(RG_INCLUDE)/linux
	@$(RG_LN) $(BUILDDIR)/os/kernel/include/asm$(if $(CONFIG_RG_UML),-i386,) $(RG_INCLUDE)/asm
	@$(RG_LN) $(BUILDDIR)/os/kernel/include/scsi $(RG_INCLUDE)/scsi
endif

