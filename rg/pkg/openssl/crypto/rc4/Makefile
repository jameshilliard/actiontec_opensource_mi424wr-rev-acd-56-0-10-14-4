RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/rc4/Makefile
#

TOP=	../..

RC4_ENC=rc4_enc.o
# or use
#RC4_ENC=asm/rx86-elf.o
#RC4_ENC=asm/rx86-out.o
#RC4_ENC=asm/rx86-sol.o
#RC4_ENC=asm/rx86bdsi.o

include ../crypto_env.mak

TEST=rc4test.c
APPS=

EXPORT_HEADERS=rc4.h

O_OBJS=rc4_skey.o $(RC4_ENC)

ifdef CONFIG_RG_PPP_MPPE
  SRC=rc4_enc.c rc4_skey.c rc4_locl.h
  CD_EXPORTED_FILES+=$(SRC)
endif

include $(RGMK)

archconfig:: 

# elf
asm/rx86-elf.o: asm/rx86unix.cpp
	$(CPP) -DELF -x c asm/rx86unix.cpp | as -o asm/rx86-elf.o

# solaris
asm/rx86-sol.o: asm/rx86unix.cpp
	$(CC) -E -DSOL asm/rx86unix.cpp | sed 's/^#.*//' > asm/rx86-sol.s
	as -o asm/rx86-sol.o asm/rx86-sol.s
	rm -f asm/rx86-sol.s

# a.out
asm/rx86-out.o: asm/rx86unix.cpp
	$(CPP) -DOUT asm/rx86unix.cpp | as -o asm/rx86-out.o

# bsdi
asm/rx86bsdi.o: asm/rx86unix.cpp
	$(CPP) -DBSDI asm/rx86unix.cpp | sed 's/ :/:/' | as -o asm/rx86bsdi.o

asm/rx86unix.cpp: asm/rc4-586.pl ../perlasm/x86asm.pl
	(cd asm; $(PERL) rc4-586.pl cpp >rx86unix.cpp)

include $(SSLMK)

