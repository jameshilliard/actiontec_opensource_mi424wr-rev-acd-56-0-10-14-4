RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/rsa/Makefile
#

TOP=	../..

include ../crypto_env.mak

TEST=rsa_test.c
APPS=
EXPORT_HEADERS=rsa.h

O_OBJS= rsa_eay.o rsa_gen.o rsa_lib.o rsa_sign.o rsa_saos.o rsa_err.o \
	rsa_pk1.o rsa_ssl.o rsa_none.o rsa_oaep.o rsa_chk.o rsa_null.o

include $(RGMK)
include $(SSLMK)
