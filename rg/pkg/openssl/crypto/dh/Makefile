RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/md4/Makefile
#

TOP=    ../..

include ../crypto_env.mak

TEST=dhtest.c
APPS=

EXPORT_HEADERS=dh.h

O_OBJS= dh_gen.o dh_key.o dh_lib.o dh_check.o dh_err.o

include $(RGMK)

include $(SSLMK)

