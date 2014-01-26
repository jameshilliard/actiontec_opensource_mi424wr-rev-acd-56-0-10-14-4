RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/x509v3/Makefile
#

TOP=	../..

include ../crypto_env.mak

TEST=
APPS=
EXPORT_HEADERS=x509v3.h

O_OBJS=v3_bcons.o v3_bitst.o v3_conf.o v3_extku.o v3_ia5.o v3_lib.o \
	v3_prn.o v3_utl.o v3err.o v3_genn.o v3_alt.o v3_skey.o v3_akey.o \
	v3_pku.o v3_int.o v3_enum.o v3_sxnet.o v3_cpols.o v3_crld.o v3_purp.o \
	v3_info.o

include $(RGMK)
include $(SSLMK)
