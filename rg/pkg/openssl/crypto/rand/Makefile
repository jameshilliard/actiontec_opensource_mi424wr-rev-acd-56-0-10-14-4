RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/rand/Makefile
#

TOP=	../..

include ../crypto_env.mak

TEST= randtest.c
APPS=
EXPORT_HEADERS=rand.h

O_OBJS=md_rand.o randfile.o rand_lib.o rand_err.o rand_egd.o rand_win.o

include $(RGMK)
include $(SSLMK)
