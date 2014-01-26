RGSRC=../../../
include $(RGSRC)/envir.mak
#
# SSLeay/rsaref/Makefile
#

DIR=	rsaref
TOP=	..
INCLUDES= -I../crypto -I../include
INSTALL_PREFIX=
OPENSSLDIR=     /usr/local/ssl
INSTALLTOP=/usr/local/ssl

CFLAGS= $(INCLUDES) $(CFLAG)

GENERAL=Makefile rsaref-lib.com install.com
TEST=
APPS=

A_TARGET=$(TOP)/libRSAglue.a
O_OBJS=rsaref.o rsar_err.o

SRC= $(LIBSRC)

EXHEADER=	rsaref.h
HEADER=	$(EXHEADER)

ALL=    $(GENERAL) $(SRC) $(HEADER)

include $(RGMK)

archconfig:: links

files:
	$(PERL) $(TOP)/util/files.pl Makefile.ssl >> $(TOP)/MINFO

links:
	@$(PERL) $(TOP)/util/mklink.pl ../include/openssl $(EXHEADER)
	@$(PERL) $(TOP)/util/mklink.pl ../test $(TEST)
	@$(PERL) $(TOP)/util/mklink.pl ../apps $(APPS)

install:
	-@if [ "x`echo x $(EX_LIBS) | grep RSAglue`" != x ]; then \
	    echo "installing libRSAglue.a"; \
	    cp $(LIB) $(INSTALL_PREFIX)$(INSTALLTOP)/lib/libRSAglue.a; \
	    $(RANLIB) $(INSTALL_PREFIX)$(INSTALLTOP)/lib/libRSAglue.a; \
	    chmod 644 $(INSTALL_PREFIX)$(INSTALLTOP)/lib/libRSAglue.a; \
	fi

#	@for i in $(EXHEADER) ; \
#	do  \
#	(cp $$i $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i; \
#	chmod 644 $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i ); \
#	done;

tags:
	ctags $(SRC)

tests:

lint:
	lint -DLINT $(INCLUDES) $(SRC)>fluff

dclean:
	$(PERL) -pe 'if (/^# DO NOT DELETE THIS LINE/) {print; exit(0);}' $(MAKEFILE) >Makefile.new
	mv -f Makefile.new $(MAKEFILE)

