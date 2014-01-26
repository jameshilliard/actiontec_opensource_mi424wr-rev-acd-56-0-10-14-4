RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/md4/Makefile
#

TOP=    ../..

CREATE_LIB=y

include ../crypto_env.mak

TEST=md4test.c
APPS=md4.c

EXPORT_HEADERS=md4.h

O_OBJS=md4_dgst.o md4_one.o

CLEAN+=asm/*.o

include $(RGMK)

include $(SSLMK)

