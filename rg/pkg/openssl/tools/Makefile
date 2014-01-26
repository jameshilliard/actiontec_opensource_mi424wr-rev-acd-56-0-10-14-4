RGSRC=../../../
include $(RGSRC)/envir.mak
#
# SSLeay/tools/Makefile
#

DIR=	tools
TOP=	..

include ../openssl_env.mak

CFLAGS+= $(INCLUDES) $(CFLAG)

GENERAL=Makefile.ssl
TEST=
APPS= c_rehash
MISC_APPS= c_hash c_info c_issuer c_name

include $(RGMK)

install:
	@for i in $(APPS) ; \
	do  \
	(cp $$i $(INSTALL_PREFIX)$(INSTALLTOP)/bin/$$i; \
	chmod 755 $(INSTALL_PREFIX)$(INSTALLTOP)/bin/$$i ); \
	done;
	@for i in $(MISC_APPS) ; \
	do  \
	(cp $$i $(INSTALL_PREFIX)$(OPENSSLDIR)/misc/$$i; \
	chmod 755 $(INSTALL_PREFIX)$(OPENSSLDIR)/misc/$$i ); \
	done;

files:
	$(PERL) $(TOP)/util/files.pl Makefile.ssl >> $(TOP)/MINFO

lint:

tags:

errors:

dclean:
	$(PERL) -pe 'if (/^# DO NOT DELETE THIS LINE/) {print; exit(0);}' $(MAKEFILE) >Makefile.new
	mv -f Makefile.new $(MAKEFILE)

errors:

