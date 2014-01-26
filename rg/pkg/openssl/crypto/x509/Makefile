RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/x509/Makefile
#

TOP=	../..

include ../crypto_env.mak

TEST=
APPS=
EXPORT_HEADERS=x509.h x509_vfy.h

O_OBJS=x509_def.o x509_d2.o x509_r2x.o x509_cmp.o \
       x509_obj.o x509_req.o x509spki.o x509_vfy.o \
       x509_set.o x509rset.o x509_err.o \
       x509name.o x509_v3.o x509_ext.o x509_att.o \
       x509type.o x509_lu.o x_all.o x509_txt.o \
       x509_trs.o by_file.o by_dir.o

include $(RGMK)
include $(SSLMK)
