# Common rules for OpenSSL Makefiles

ifdef IS_BUILDDIR
archconfig:: links

links: $(TEST) $(APPS)
	@$(PERL) $(TOP)/util/mklink.pl $(TOP)/test $(TEST)
	@$(PERL) $(TOP)/util/mklink.pl $(TOP)/apps $(APPS)

tests:

.PHONY: tests links
endif
