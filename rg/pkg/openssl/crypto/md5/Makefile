RGSRC=../../../../
include $(RGSRC)/envir.mak
#
# SSLeay/crypto/md5/Makefile
#

TOP=    ../..

CREATE_LIB=y

include ../crypto_env.mak

# We let the C compiler driver to take care of .s files. This is done in
# order to be excused from maintaining a separate set of architecture
# dependent assembler flags. E.g. if you throw -mcpu=ultrasparc at SPARC
# gcc, then the driver will automatically translate it to -xarch=v8plus
# and pass it down to assembler.
AS=$(CC) -c
ASFLAGS=$(CFLAGS)

TEST=md5test.c
APPS=
EXPORT_HEADERS=md5.h

O_OBJS=md5_dgst.o md5_one.o $(MD5_ASM_OBJ)
CLEAN+=asm/*.o

include $(RGMK)

# elf
asm/mx86-elf.o: asm/mx86unix.cpp
	$(CPP) -DELF -x c asm/mx86unix.cpp | as -o asm/mx86-elf.o

# solaris
asm/mx86-sol.o: asm/mx86unix.cpp
	$(CC) -E -DSOL asm/mx86unix.cpp | sed 's/^#.*//' > asm/mx86-sol.s
	as -o asm/mx86-sol.o asm/mx86-sol.s
	rm -f asm/mx86-sol.s

# a.out
asm/mx86-out.o: asm/mx86unix.cpp
	$(CPP) -DOUT asm/mx86unix.cpp | as -o asm/mx86-out.o

# bsdi
asm/mx86bsdi.o: asm/mx86unix.cpp
	$(CPP) -DBSDI asm/mx86unix.cpp | sed 's/ :/:/' | as -o asm/mx86bsdi.o

asm/mx86unix.cpp: asm/md5-586.pl ../perlasm/x86asm.pl
	(cd asm; $(PERL) md5-586.pl cpp >mx86unix.cpp)

asm/md5-sparcv8plus.o: asm/md5-sparcv9.S
	$(CC) $(ASFLAGS) -DMD5_BLOCK_DATA_ORDER -c \
		-o asm/md5-sparcv8plus.o asm/md5-sparcv9.S

# Old GNU assembler doesn't understand V9 instructions, so we
# hire /usr/ccs/bin/as to do the job. Note that option is called
# *-gcc27, but even gcc 2>=8 users may experience similar problem
# if they didn't bother to upgrade GNU assembler. Such users should
# not choose this option, but be adviced to *remove* GNU assembler
# or upgrade it.
asm/md5-sparcv8plus-gcc27.o: asm/md5-sparcv9.S
	$(CC) $(ASFLAGS) -DMD5_BLOCK_DATA_ORDER -E asm/md5-sparcv9.S | \
		/usr/ccs/bin/as -xarch=v8plus - -o asm/md5-sparcv8plus-gcc27.o

asm/md5-sparcv9.o: asm/md5-sparcv9.S
	$(CC) $(ASFLAGS) -DMD5_BLOCK_DATA_ORDER -c \
		-o asm/md5-sparcv9.o asm/md5-sparcv9.S

include $(SSLMK)
