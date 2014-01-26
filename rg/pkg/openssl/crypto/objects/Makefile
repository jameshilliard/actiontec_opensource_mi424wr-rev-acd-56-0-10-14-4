RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/objects/Makefile
#

TOP=	../..
include ../crypto_env.mak

TEST=
APPS=
EXPORT_HEADERS=objects.h obj_mac.h
CD_EXPORTED_FILES+=obj_dat.pl objects.pl objects.txt obj_mac.num.orig

O_OBJS= o_names.o obj_dat.o obj_lib.o obj_err.o

include $(RGMK)

archconfig:: obj_dat.h

obj_dat.h: obj_dat.pl obj_mac.h
	$(PERL) obj_dat.pl obj_mac.h obj_dat.h

# objects.pl both reads and writes obj_mac.num
obj_mac.h: objects.pl objects.txt obj_mac.num
	$(PERL) objects.pl objects.txt obj_mac.num obj_mac.h

obj_mac.num: obj_mac.num.orig
	@cp $< $@

include $(SSLMK)
