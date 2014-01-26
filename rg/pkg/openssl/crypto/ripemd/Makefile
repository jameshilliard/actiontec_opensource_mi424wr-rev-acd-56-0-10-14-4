RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/ripemd/Makefile
#

TOP=    ../..

include ../crypto_env.mak

TEST=rmdtest.c
APPS=
EXPORT_HEADERS=ripemd.h

O_OBJS=rmd_dgst.o rmd_one.o $(RMD160_ASM_OBJ)
CLEAN+=asm/*.o

include $(RGMK)

# elf
asm/rm86-elf.o: asm/rm86unix.cpp
	$(CPP) -DELF -x c asm/rm86unix.cpp | as -o asm/rm86-elf.o

# solaris
asm/rm86-sol.o: asm/rm86unix.cpp
	$(CC) -E -DSOL asm/rm86unix.cpp | sed 's/^#.*//' > asm/rm86-sol.s
	as -o asm/rm86-sol.o asm/rm86-sol.s
	rm -f asm/rm86-sol.s

# a.out
asm/rm86-out.o: asm/rm86unix.cpp
	$(CPP) -DOUT asm/rm86unix.cpp | as -o asm/rm86-out.o

# bsdi
asm/rm86bsdi.o: asm/rm86unix.cpp
	$(CPP) -DBSDI asm/rm86unix.cpp | sed 's/ :/:/' | as -o asm/rm86bsdi.o

asm/rm86unix.cpp: asm/rmd-586.pl ../perlasm/x86asm.pl
	(cd asm; $(PERL) rmd-586.pl cpp >rm86unix.cpp)

include $(SSLMK)
