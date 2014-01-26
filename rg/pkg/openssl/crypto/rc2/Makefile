RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/rc2/Makefile
#

TOP=	../..

include ../crypto_env.mak

TEST=rc2test.c
APPS=

EXPORT_HEADERS=rc2.h

O_OBJS=rc2_ecb.o rc2_skey.o rc2_cbc.o rc2cfb64.o rc2ofb64.o

include $(RGMK)

archconfig:: 

include $(SSLMK)

