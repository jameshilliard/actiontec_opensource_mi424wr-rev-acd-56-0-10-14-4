RGSRC=../../../
include $(RGSRC)/envir.mak
#
#  apps/Makefile.ssl
#

TOP=		..
INCLUDES=	-I../include

PEX_LIBS=
EX_LIBS= 
EXE_EXT= 

CFLAGS+= -DMONOLITH $(INCLUDES) $(CFLAG)

GENERAL=Makefile makeapps.com install.com

DLIBCRYPTO=$(TOP)/libcrypto.a
DLIBSSL=$(TOP)/libssl.a
LIBCRYPTO=-L$(TOP) -lcrypto
LIBSSL=-L$(TOP) -lssl

PROGRAM= openssl

SCRIPTS=CA.sh CA.pl der_chop

E_EXE=	verify asn1pars req dgst dh dhparam enc passwd gendh errstr \
	ca crl rsa rsautl dsa dsaparam \
	x509 genrsa gendsa s_server s_client speed \
	s_time version pkcs7 crl2pkcs7 sess_id ciphers nseq pkcs12 \
	pkcs8 spkac smime rand

PROGS= $(PROGRAM).c

A_OBJ=apps.o
A_SRC=apps.c
S_OBJ=	s_cb.o s_socket.o
S_SRC=	s_cb.c s_socket.c
RAND_OBJ=app_rand.o
RAND_SRC=app_rand.c

E_OBJ=	verify.o asn1pars.o req.o dgst.o dh.o dhparam.o enc.o passwd.o gendh.o errstr.o \
	ca.o pkcs7.o crl2p7.o crl.o \
	rsa.o rsautl.o dsa.o dsaparam.o \
	x509.o genrsa.o gendsa.o s_server.o s_client.o speed.o \
	s_time.o $(A_OBJ) $(S_OBJ) $(RAND_OBJ) version.o sess_id.o \
	ciphers.o nseq.o pkcs12.o pkcs8.o spkac.o smime.o rand.o

EXPORT_HEADERS_DIR=openssl
EXPORT_HEADERS+=apps.h

FIRST_TARGETS=$(PROGRAM) rehash
OTHER_TARGETS=$(E_EXE)

O_OBJS_$(PROGRAM)=$(E_OBJ) $(PROGRAM).o

include $(RGMK)

ifdef IS_BUILDDIR

archconfig:: progs.h

req: sreq.o $(A_OBJ) $(DLIBCRYPTO)
	$(CC) -o $@ $(CFLAG) sreq.o $(A_OBJ) $(RAND_OBJ) \
	    $(PEX_LIBS) $(LIBCRYPTO) $(EX_LIBS)

sreq.o: req.c 

$(PROGRAM): progs.h $(E_OBJ) $(PROGRAM).o $(DLIBCRYPTO) $(DLIBSSL)
	
rehash: $(PROGRAM)
	-(cd $(TOP); OPENSSL="`pwd`/apps/openssl"; export OPENSSL; \
	  $(PERL) tools/c_rehash certs)

progs.h: progs.pl
	$(PERL) progs.pl $(E_EXE) >progs.h
	$(RM) $(PROGRAM).o
endif
