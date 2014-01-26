RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/bn/Makefile
#

TOP=	../..

include ../crypto_env.mak

# We let the C compiler driver to take care of .s files. This is done in
# order to be excused from maintaining a separate set of architecture
# dependent assembler flags. E.g. if you throw -mcpu=ultrasparc at SPARC
# gcc, then the driver will automatically translate it to -xarch=v8plus
# and pass it down to assembler.
AS=$(CC) -c
ASFLAGS=$(CFLAGS)

TEST=bntest.c exptest.c
APPS=
EXPORT_HEADERS=bn.h

O_OBJS= bn_add.o bn_div.o bn_exp.o bn_lib.o bn_ctx.o bn_mul.o \
	bn_print.o bn_rand.o bn_shift.o bn_word.o bn_blind.o \
	bn_gcd.o bn_prime.o bn_err.o bn_sqr.o $(BN_ASM) bn_recp.o bn_mont.o \
	bn_mpi.o bn_exp2.o


CLEAN+=asm/*.o
CD_EXPORTED_FILES+=bn_prime.pl

include $(RGMK)

archconfig:: bn_prime.h

bn_prime.h: bn_prime.pl
	$(PERL) bn_prime.pl >bn_prime.h

# elf
asm/bn86-elf.o: asm/bn86unix.cpp
	$(CPP) -DELF -x c asm/bn86unix.cpp | as -o asm/bn86-elf.o

asm/co86-elf.o: asm/co86unix.cpp
	$(CPP) -DELF -x c asm/co86unix.cpp | as -o asm/co86-elf.o

# solaris
asm/bn86-sol.o: asm/bn86unix.cpp
	$(CC) -E -DSOL asm/bn86unix.cpp | sed 's/^#.*//' > asm/bn86-sol.s
	as -o asm/bn86-sol.o asm/bn86-sol.s
	rm -f asm/bn86-sol.s

asm/co86-sol.o: asm/co86unix.cpp
	$(CC) -E -DSOL asm/co86unix.cpp | sed 's/^#.*//' > asm/co86-sol.s
	as -o asm/co86-sol.o asm/co86-sol.s
	rm -f asm/co86-sol.s

# a.out
asm/bn86-out.o: asm/bn86unix.cpp
	$(CPP) -DOUT asm/bn86unix.cpp | as -o asm/bn86-out.o

asm/co86-out.o: asm/co86unix.cpp
	$(CPP) -DOUT asm/co86unix.cpp | as -o asm/co86-out.o

# bsdi
asm/bn86bsdi.o: asm/bn86unix.cpp
	$(CPP) -DBSDI asm/bn86unix.cpp | sed 's/ :/:/' | as -o asm/bn86bsdi.o

asm/co86bsdi.o: asm/co86unix.cpp
	$(CPP) -DBSDI asm/co86unix.cpp | sed 's/ :/:/' | as -o asm/co86bsdi.o

asm/bn86unix.cpp: asm/bn-586.pl ../perlasm/x86asm.pl
	(cd asm; $(PERL) bn-586.pl cpp >bn86unix.cpp )

asm/co86unix.cpp: asm/co-586.pl ../perlasm/x86asm.pl
	(cd asm; $(PERL) co-586.pl cpp >co86unix.cpp )

asm/sparcv8.o: asm/sparcv8.S

asm/sparcv8plus.o: asm/sparcv8plus.S

# Old GNU assembler doesn't understand V9 instructions, so we
# hire /usr/ccs/bin/as to do the job. Note that option is called
# *-gcc27, but even gcc 2>=8 users may experience similar problem
# if they didn't bother to upgrade GNU assembler. Such users should
# not choose this option, but be adviced to *remove* GNU assembler
# or upgrade it.
asm/sparcv8plus-gcc27.o: asm/sparcv8plus.S
	$(CC) $(ASFLAGS) -E asm/sparcv8plus.S | \
		/usr/ccs/bin/as -xarch=v8plus - -o asm/sparcv8plus-gcc27.o

asm/ia64.o: asm/ia64.S

include $(SSLMK)
