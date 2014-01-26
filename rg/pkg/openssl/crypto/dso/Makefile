RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/dso/Makefile
#

TOP=	../..

include ../crypto_env.mak

TEST=
APPS=
EXPORT_HEADERS=dso.h

O_OBJS=dso_dl.o dso_err.o dso_lib.o dso_null.o \
  dso_openssl.o dso_win32.o dso_vms.o

ifdef CONFIG_DYN_LINK
  O_OBJS+=dso_dlfcn.o 
endif
include $(RGMK)
include $(SSLMK)
