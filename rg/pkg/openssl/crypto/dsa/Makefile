RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/dsa/Makefile
#

TOP=	../..

include ../crypto_env.mak

TEST=dsatest.c
APPS=
EXPORT_HEADERS=dsa.h

O_OBJS= dsa_gen.o dsa_key.o dsa_lib.o dsa_asn1.o dsa_vrf.o dsa_sign.o \
	dsa_err.o dsa_ossl.o

include $(RGMK)
include $(SSLMK)
