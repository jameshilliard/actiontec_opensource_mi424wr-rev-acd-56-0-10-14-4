RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/pkcs12/Makefile
#

TOP=	../..

include ../crypto_env.mak

TEST=
APPS=
EXPORT_HEADERS=pkcs12.h

O_OBJS=p12_add.o p12_attr.o p12_bags.o p12_crpt.o p12_crt.o p12_decr.o \
	p12_init.o p12_key.o p12_kiss.o p12_lib.o p12_mac.o p12_mutl.o\
	p12_sbag.o p12_utl.o p12_npas.o pk12err.o

include $(RGMK)
include $(SSLMK)
