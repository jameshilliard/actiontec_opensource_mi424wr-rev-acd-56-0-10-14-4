RGSRC=../../../
include $(RGSRC)/envir.mak
#
# test/Makefile.ssl
#

DIR=		test
TOP=		..
INCLUDES=	-I../include
INSTALL_PREFIX=

PEX_LIBS=
EX_LIBS= #-lnsl -lsocket

CFLAGS+= $(INCLUDES) $(CFLAG)

GENERAL=Makefile.ssl maketests.com \
	tests.com testenc.com tx509.com trsa.com tcrl.com tsid.com treq.com \
	tpkcs7.com tpkcs7d.com tverify.com testgen.com testss.com testssl.com \
	testca.com VMSca-response.1 VMSca-response.2

DLIBCRYPTO= ../libcrypto.a
DLIBSSL= ../libssl.a
LIBCRYPTO= -L.. -lcrypto
LIBSSL= -L.. -lssl

BNTEST=		bntest
EXPTEST=	exptest
IDEATEST=	ideatest
SHATEST=	shatest
SHA1TEST=	sha1test
MDC2TEST=	mdc2test
RMDTEST=	rmdtest
MD2TEST=	md2test
MD4TEST=	md4test
MD5TEST=	md5test
HMACTEST=	hmactest
RC2TEST=	rc2test
RC4TEST=	rc4test
RC5TEST=	rc5test
BFTEST=		bftest
CASTTEST=	casttest
DESTEST=	destest
RANDTEST=	randtest
DHTEST=		dhtest
DSATEST=	dsatest
METHTEST=	methtest
SSLTEST=	ssltest
RSATEST=	rsa_test

EXE=	$(BNTEST) $(MD5TEST) $(SHATEST) $(SHA1TEST) $(RMDTEST) \
	$(RANDTEST) $(SSLTEST) $(EXPTEST) $(DSATEST) $(RSATEST)

EXHEADER= 
HEADER=	$(EXHEADER)

ALL=    $(GENERAL) $(SRC) $(HEADER)

OTHER_TARGETS=$(EXE)

include $(RGMK)

files:
	$(PERL) $(TOP)/util/files.pl Makefile.ssl >> $(TOP)/MINFO

errors:

install:

tags:
	ctags $(SRC)

tests:	exe apps \
	test_des test_sha test_md5 \
	\
	test_rmd test_rc2 test_rc4 test_rc5 test_bf test_cast \
	test_rand test_bn test_enc test_x509 test_rsa test_crl test_sid \
	test_gen test_req test_pkcs7 test_verify test_dh test_dsa \
	test_ss test_ca test_ssl

apps:
	@(cd ../apps; $(MAKE)  CC='${CC}' CFLAG='${CFLAG}' INSTALLTOP='${INSTALLTOP}' PEX_LIBS='${PEX_LIBS}' EX_LIBS='${EX_LIBS}' all)

test_des:
	./$(DESTEST)

test_idea:
	./$(IDEATEST)

test_sha:
	./$(SHATEST)
	./$(SHA1TEST)

test_mdc2:
	./$(MDC2TEST)

test_md5:
	./$(MD5TEST)

test_md4:
	./$(MD4TEST)

test_hmac:
	./$(HMACTEST)

test_md2:
	./$(MD2TEST)

test_rmd:
	./$(RMDTEST)

test_bf:
	./$(BFTEST)

test_cast:
	./$(CASTTEST)

test_rc2:
	./$(RC2TEST)

test_rc4:
	./$(RC4TEST)

test_rc5:
	./$(RC5TEST)

test_rand:
	./$(RANDTEST)

test_enc:
	@sh ./testenc

test_x509:
	echo test normal x509v1 certificate
	sh ./tx509 2>/dev/null
	echo test first x509v3 certificate
	sh ./tx509 v3-cert1.pem 2>/dev/null
	echo test second x509v3 certificate
	sh ./tx509 v3-cert2.pem 2>/dev/null

test_rsa:
	@sh ./trsa 2>/dev/null
	./$(RSATEST)

test_crl:
	@sh ./tcrl 2>/dev/null

test_sid:
	@sh ./tsid 2>/dev/null

test_req:
	@sh ./treq 2>/dev/null
	@sh ./treq testreq2.pem 2>/dev/null

test_pkcs7:
	@sh ./tpkcs7 2>/dev/null
	@sh ./tpkcs7d 2>/dev/null

test_bn:
	@echo starting big number library test, could take a while...
	@./$(BNTEST) >tmp.bntest
	@echo quit >>tmp.bntest
	@echo "running bc"
	@<tmp.bntest sh -c "`sh ./bctest ignore`" | $(PERL) -e '$$i=0; while (<STDIN>) {if (/^test (.*)/) {print STDERR "\nverify $$1";} elsif (!/^0$$/) {die "\nFailed! bc: $$_";} else {print STDERR "."; $$i++;}} print STDERR "\n$$i tests passed\n"'
	@echo 'test a^b%c implementations'
	./$(EXPTEST)

test_verify:
	@echo "The following command should have some OK's and some failures"
	@echo "There are definitly a few expired certificates"
	../apps/openssl verify -CApath ../certs ../certs/*.pem

test_dh:
	@echo "Generate a set of DH parameters"
	./$(DHTEST)

test_dsa:
	@echo "Generate a set of DSA parameters"
	./$(DSATEST)
	./$(DSATEST) -app2_1

test_gen:
	@echo "Generate and verify a certificate request"
	@sh ./testgen

test_ss keyU.ss certU.ss certCA.ss: testss
	@echo "Generate and certify a test certificate"
	@sh ./testss

test_ssl: keyU.ss certU.ss certCA.ss
	@echo "test SSL protocol"
	@sh ./testssl keyU.ss certU.ss certCA.ss

test_ca:
	@if ../apps/openssl no-rsa; then \
	  echo "skipping CA.sh test -- requires RSA"; \
	else \
	  echo "Generate and certify a test certificate via the 'ca' program"; \
	  sh ./testca; \
 	fi

lint:
	lint -DLINT $(INCLUDES) $(SRC)>fluff

dclean:
	$(PERL) -pe 'if (/^# DO NOT DELETE THIS LINE/) {print; exit(0);}' $(MAKEFILE) >Makefile.new
	mv -f Makefile.new $(MAKEFILE)

$(DLIBSSL):
	(cd ../ssl; $(MAKE))

$(DLIBCRYPTO):
	(cd ../crypto; $(MAKE))

$(RSATEST): $(RSATEST).o $(DLIBCRYPTO)
	$(CC) -o $(RSATEST) $(CFLAGS) $(RSATEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(BNTEST): $(BNTEST).o $(DLIBCRYPTO)
	$(CC) -o $(BNTEST) $(CFLAGS) $(BNTEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(EXPTEST): $(EXPTEST).o $(DLIBCRYPTO)
	$(CC) -o $(EXPTEST) $(CFLAGS) $(EXPTEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(IDEATEST): $(IDEATEST).o $(DLIBCRYPTO)
	$(CC) -o $(IDEATEST) $(CFLAGS) $(IDEATEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(MD2TEST): $(MD2TEST).o $(DLIBCRYPTO)
	$(CC) -o $(MD2TEST) $(CFLAGS) $(MD2TEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(SHATEST): $(SHATEST).o $(DLIBCRYPTO)
	$(CC) -o $(SHATEST) $(CFLAGS) $(SHATEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(SHA1TEST): $(SHA1TEST).o $(DLIBCRYPTO)
	$(CC) -o $(SHA1TEST) $(CFLAGS) $(SHA1TEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(RMDTEST): $(RMDTEST).o $(DLIBCRYPTO)
	$(CC) -o $(RMDTEST) $(CFLAGS) $(RMDTEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(MDC2TEST): $(MDC2TEST).o $(DLIBCRYPTO)
	$(CC) -o $(MDC2TEST) $(CFLAGS) $(MDC2TEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(MD4TEST): $(MD4TEST).o $(DLIBCRYPTO)
	$(CC) -o $(MD4TEST) $(CFLAGS) $(MD4TEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(MD5TEST): $(MD5TEST).o $(DLIBCRYPTO)
	$(CC) -o $(MD5TEST) $(CFLAGS) $(MD5TEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(HMACTEST): $(HMACTEST).o $(DLIBCRYPTO)
	$(CC) -o $(HMACTEST) $(CFLAGS) $(HMACTEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(RC2TEST): $(RC2TEST).o $(DLIBCRYPTO)
	$(CC) -o $(RC2TEST) $(CFLAGS) $(RC2TEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(BFTEST): $(BFTEST).o $(DLIBCRYPTO)
	$(CC) -o $(BFTEST) $(CFLAGS) $(BFTEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(CASTTEST): $(CASTTEST).o $(DLIBCRYPTO)
	$(CC) -o $(CASTTEST) $(CFLAGS) $(CASTTEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(RC4TEST): $(RC4TEST).o $(DLIBCRYPTO)
	$(CC) -o $(RC4TEST) $(CFLAGS) $(RC4TEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(RC5TEST): $(RC5TEST).o $(DLIBCRYPTO)
	$(CC) -o $(RC5TEST) $(CFLAGS) $(RC5TEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(DESTEST): $(DESTEST).o $(DLIBCRYPTO)
	$(CC) -o $(DESTEST) $(CFLAGS) $(DESTEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(RANDTEST): $(RANDTEST).o $(DLIBCRYPTO)
	$(CC) -o $(RANDTEST) $(CFLAGS) $(RANDTEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(DHTEST): $(DHTEST).o $(DLIBCRYPTO)
	$(CC) -o $(DHTEST) $(CFLAGS) $(DHTEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(DSATEST): $(DSATEST).o $(DLIBCRYPTO)
	$(CC) -o $(DSATEST) $(CFLAGS) $(DSATEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(METHTEST): $(METHTEST).o $(DLIBCRYPTO)
	$(CC) -o $(METHTEST) $(CFLAGS) $(METHTEST).o $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

$(SSLTEST): $(SSLTEST).o $(DLIBSSL) $(DLIBCRYPTO)
	$(CC) -o $(SSLTEST) $(CFLAGS) $(SSLTEST).o $(PEX_LIBS) $(LIBSSL) $(LIBCRYPTO) $(EX_LIBS)

