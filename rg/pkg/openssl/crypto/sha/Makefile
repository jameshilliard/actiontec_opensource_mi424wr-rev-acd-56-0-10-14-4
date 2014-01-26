RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/sha/Makefile
#

TOP=    ../..

CREATE_LIB=y

include ../crypto_env.mak

TEST=shatest.c sha1test.c
APPS=
EXPORT_HEADERS=sha.h sha1dgst.c__c/sha1dgst.c

O_OBJS=sha_dgst.o sha1dgst.o sha_one.o sha1_one.o $(SHA1_ASM_OBJ)
CLEAN+=asm/*.o

include $(RGMK)

# elf
asm/sx86-elf.o: asm/sx86unix.cpp
	$(CPP) -DELF -x c asm/sx86unix.cpp | as -o asm/sx86-elf.o

# solaris
asm/sx86-sol.o: asm/sx86unix.cpp
	$(CC) -E -DSOL asm/sx86unix.cpp | sed 's/^#.*//' > asm/sx86-sol.s
	as -o asm/sx86-sol.o asm/sx86-sol.s
	rm -f asm/sx86-sol.s

# a.out
asm/sx86-out.o: asm/sx86unix.cpp
	$(CPP) -DOUT asm/sx86unix.cpp | as -o asm/sx86-out.o

# bsdi
asm/sx86bsdi.o: asm/sx86unix.cpp
	$(CPP) -DBSDI asm/sx86unix.cpp | sed 's/ :/:/' | as -o asm/sx86bsdi.o

asm/sx86unix.cpp: asm/sha1-586.pl ../perlasm/x86asm.pl
	(cd asm; $(PERL) sha1-586.pl cpp $(PROCESSOR) >sx86unix.cpp)

include $(SSLMK)
