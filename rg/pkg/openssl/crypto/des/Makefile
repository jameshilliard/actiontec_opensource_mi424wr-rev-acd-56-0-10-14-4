RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/des/Makefile
#

TOP=	../..

DES_ENC=	des_enc.o fcrypt_b.o
# or use
#DES_ENC=	dx86-elf.o yx86-elf.o

CREATE_LIB=y
include ../crypto_env.mak

TEST=destest.c
APPS=

EXPORT_HEADERS=des.h

O_OBJS= set_key.o  ecb_enc.o  cbc_enc.o \
	ecb3_enc.o cfb64enc.o cfb64ede.o cfb_enc.o  ofb64ede.o \
	enc_read.o enc_writ.o ofb64enc.o \
	ofb_enc.o  str2key.o  pcbc_enc.o qud_cksm.o rand_key.o \
	${DES_ENC} read2pwd.o \
	fcrypt.o xcbc_enc.o read_pwd.o rpc_enc.o  cbc_cksm.o \
	ede_cbcm_enc.o

CLEAN+=asm/*.o

include $(RGMK)

# elf
asm/dx86-elf.o: asm/dx86unix.cpp
	$(CPP) -DELF -x c asm/dx86unix.cpp | as -o asm/dx86-elf.o

asm/yx86-elf.o: asm/yx86unix.cpp
	$(CPP) -DELF -x c asm/yx86unix.cpp | as -o asm/yx86-elf.o

# solaris
asm/dx86-sol.o: asm/dx86unix.cpp
	$(CC) -E -DSOL asm/dx86unix.cpp | sed 's/^#.*//' > asm/dx86-sol.s
	as -o asm/dx86-sol.o asm/dx86-sol.s
	rm -f asm/dx86-sol.s

asm/yx86-sol.o: asm/yx86unix.cpp
	$(CC) -E -DSOL asm/yx86unix.cpp | sed 's/^#.*//' > asm/yx86-sol.s
	as -o asm/yx86-sol.o asm/yx86-sol.s
	rm -f asm/yx86-sol.s

# a.out
asm/dx86-out.o: asm/dx86unix.cpp
	$(CPP) -DOUT asm/dx86unix.cpp | as -o asm/dx86-out.o

asm/yx86-out.o: asm/yx86unix.cpp
	$(CPP) -DOUT asm/yx86unix.cpp | as -o asm/yx86-out.o

# bsdi
asm/dx86bsdi.o: asm/dx86unix.cpp
	$(CPP) -DBSDI asm/dx86unix.cpp | sed 's/ :/:/' | as -o asm/dx86bsdi.o

asm/yx86bsdi.o: asm/yx86unix.cpp
	$(CPP) -DBSDI asm/yx86unix.cpp | sed 's/ :/:/' | as -o asm/yx86bsdi.o

asm/dx86unix.cpp: asm/des-586.pl ../perlasm/x86asm.pl ../perlasm/cbc.pl
	(cd asm; $(PERL) des-586.pl cpp >dx86unix.cpp)

asm/yx86unix.cpp: asm/crypt586.pl ../perlasm/x86asm.pl
	(cd asm; $(PERL) crypt586.pl cpp >yx86unix.cpp)

include $(SSLMK)

