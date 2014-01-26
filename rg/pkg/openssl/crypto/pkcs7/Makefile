RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/pkcs7/Makefile
#

TOP=	../..

include ../crypto_env.mak

TEST=
APPS=
EXPORT_HEADERS=pkcs7.h

O_OBJS= pk7_lib.o pkcs7err.o pk7_doit.o pk7_smime.o pk7_attr.o pk7_mime.o

ifdef TEST
TESTAPPS=enc dec sign verify
TARGET+=$(TESTAPPS)
LDFLAGS+=-L$(TOP)
LDLIBS:=-lcrypto $(LDLIBS)

O_OBJS_enc=enc.o
O_OBJS_dec=dec.o
O_OBJS_verify=verify.o example.o
O_OBJS_sign=sign.o
endif

include $(RGMK)
include $(SSLMK)
