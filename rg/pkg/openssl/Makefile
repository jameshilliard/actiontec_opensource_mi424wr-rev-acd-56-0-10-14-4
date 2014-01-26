RGSRC=../../
include $(RGSRC)/envir.mak

VERSION=0.9.6d
MAJOR=0
MINOR=9.6
SHLIB_VERSION_NUMBER=0.9.6
SHLIB_VERSION_HISTORY=
SHLIB_MAJOR=0
SHLIB_MINOR=9.6
SHLIB_EXT=.so.$(SHLIB_MAJOR).$(SHLIB_MINOR)
SHLIB_TARGET=linux-shared

TOP=.

include openssl_env.mak

# INSTALL_PREFIX is for package builders so that they can configure
# for, say, /usr/ and yet have everything installed to /tmp/somedir/usr/.
# Normally it is left empty.
#INSTALL_PREFIX=
#INSTALLTOP=/usr/local/ssl

# Do not edit this manually. Use Configure --openssldir=DIR do change this!
#OPENSSLDIR=/usr/local/ssl

# RSAref  - Define if we are to link with RSAref.
# NO_IDEA - Define to build without the IDEA algorithm
# NO_RC4  - Define to build without the RC4 algorithm
# NO_RC2  - Define to build without the RC2 algorithm
# THREADS - Define when building with threads, you will probably also need any
#           system defines as well, i.e. _REENTERANT for Solaris 2.[34]
# TERMIO  - Define the termio terminal subsystem, needed if sgtty is missing.
# TERMIOS - Define the termios terminal subsystem, Silicon Graphics.
# LONGCRYPT - Define to use HPUX 10.x's long password modification to crypt(3).
# DEVRANDOM - Give this the value of the 'random device' if your OS supports
#           one.  32 bytes will be read from this when the random
#           number generator is initalised.
# SSL_FORBID_ENULL - define if you want the server to be not able to use the
#           NULL encryption ciphers.
#
# LOCK_DEBUG - turns on lots of lock debug output :-)
# REF_CHECK - turn on some xyz_free() assertions.
# REF_PRINT - prints some stuff on structure free.
# CRYPTO_MDEBUG - turns on my 'memory leak' detecting stuff
# MFUNC - Make all Malloc/Free/Realloc calls call
#       CRYPTO_malloc/CRYPTO_free/CRYPTO_realloc which can be setup to
#       call application defined callbacks via CRYPTO_set_mem_functions()
# MD5_ASM needs to be defined to use the x86 assembler for MD5
# SHA1_ASM needs to be defined to use the x86 assembler for SHA1
# RMD160_ASM needs to be defined to use the x86 assembler for RIPEMD160
# Do not define B_ENDIAN or L_ENDIAN if 'unsigned long' == 8.  It must
# equal 4.
# PKCS1_CHECK - pkcs1 tests.

#CC= gcc
#CFLAG= -DL_ENDIAN -DTERMIO -O3 -fomit-frame-pointer -m486 -Wall -Wuninitialized -DSHA1_ASM -DMD5_ASM -DRMD160_ASM

# XXX - this flag currently has no effects, depends on bug B1514 to be fixed
DEPFLAG= -DNO_BF -DNO_CAST -DNO_DES -DNO_MDC2 -DNO_DH -DNO_HMAC -DNO_MD2 \
    -DNO_MD4 -DNO_RC2 -DNO_RC4 -DNO_RC5 -DNO_IDEA

PEX_LIBS= 
EXE_EXT= 
TAR= tar
TARFLAGS= --no-recursion

# Set BN_ASM to bn_asm.o if you want to use the C version
#BN_ASM= bn_asm.o
#BN_ASM= asm/bn86-elf.o	# elf, linux-elf
#BN_ASM= asm/bn86-sol.o # solaris
#BN_ASM= asm/bn86-out.o # a.out, FreeBSD
#BN_ASM= asm/bn86bsdi.o # bsdi
#BN_ASM= asm/alpha.o    # DEC Alpha
#BN_ASM= asm/pa-risc2.o # HP-UX PA-RISC
#BN_ASM= asm/r3000.o    # SGI MIPS cpu
#BN_ASM= asm/sparc.o    # Sun solaris/SunOS
#BN_ASM= asm/bn-win32.o # Windows 95/NT
#BN_ASM= asm/x86w16.o   # 16 bit code for Windows 3.1/DOS
#BN_ASM= asm/x86w32.o   # 32 bit code for Windows 3.1

# For x86 assembler: Set PROCESSOR to 386 if you want to support
# the 80386.
#PROCESSOR= 

# Set DES_ENC to des_enc.o if you want to use the C version
#There are 4 x86 assember options.
#DES_ENC= des_enc.o fcrypt_b.o          # C
#DES_ENC= asm/dx86-elf.o asm/yx86-elf.o # elf
#DES_ENC= asm/dx86-sol.o asm/yx86-sol.o # solaris
#DES_ENC= asm/dx86-out.o asm/yx86-out.o # a.out, FreeBSD
#DES_ENC= asm/dx86bsdi.o asm/yx86bsdi.o # bsdi

# Set BF_ENC to bf_enc.o if you want to use the C version
#There are 4 x86 assember options.
#BF_ENC= bf_enc.o
#BF_ENC= asm/bx86-elf.o # elf
#BF_ENC= asm/bx86-sol.o # solaris
#BF_ENC= asm/bx86-out.o # a.out, FreeBSD
#BF_ENC= asm/bx86bsdi.o # bsdi

# Set CAST_ENC to c_enc.o if you want to use the C version
#There are 4 x86 assember options.
#CAST_ENC= c_enc.o
#CAST_ENC= asm/cx86-elf.o # elf
#CAST_ENC= asm/cx86-sol.o # solaris
#CAST_ENC= asm/cx86-out.o # a.out, FreeBSD
#CAST_ENC= asm/cx86bsdi.o # bsdi

# Set RC4_ENC to rc4_enc.o if you want to use the C version
#There are 4 x86 assember options.
#RC4_ENC= rc4_enc.o
#RC4_ENC= asm/rx86-elf.o # elf
#RC4_ENC= asm/rx86-sol.o # solaris
#RC4_ENC= asm/rx86-out.o # a.out, FreeBSD
#RC4_ENC= asm/rx86bsdi.o # bsdi

# Set RC5_ENC to rc5_enc.o if you want to use the C version
#There are 4 x86 assember options.
#RC5_ENC= rc5_enc.o
#RC5_ENC= asm/r586-elf.o # elf
#RC5_ENC= asm/r586-sol.o # solaris
#RC5_ENC= asm/r586-out.o # a.out, FreeBSD
#RC5_ENC= asm/r586bsdi.o # bsdi

# Also need MD5_ASM defined
#MD5_ASM_OBJ= asm/mx86-elf.o        # elf
#MD5_ASM_OBJ= asm/mx86-sol.o        # solaris
#MD5_ASM_OBJ= asm/mx86-out.o        # a.out, FreeBSD
#MD5_ASM_OBJ= asm/mx86bsdi.o        # bsdi

# Also need SHA1_ASM defined
#SHA1_ASM_OBJ= asm/sx86-elf.o       # elf
#SHA1_ASM_OBJ= asm/sx86-sol.o       # solaris
#SHA1_ASM_OBJ= asm/sx86-out.o       # a.out, FreeBSD
#SHA1_ASM_OBJ= asm/sx86bsdi.o       # bsdi

# Also need RMD160_ASM defined
#RMD160_ASM_OBJ= asm/rm86-elf.o       # elf
#RMD160_ASM_OBJ= asm/rm86-sol.o       # solaris
#RMD160_ASM_OBJ= asm/rm86-out.o       # a.out, FreeBSD
#RMD160_ASM_OBJ= asm/rm86bsdi.o       # bsdi

# When we're prepared to use shared libraries in the programs we link here
# we might set SHLIB_MARK to '$(SHARED_LIBS)'.
SHLIB_MARK=

# XXX Currently only the crypto is used
# should always compile on all configurations
SUBDIRS=crypto #rsaref $(SHLIB_MARK) apps test tools
SHLIBDIRS=crypto

# go throu all directories just to export header files.
ARCHCONFIG_SUBDIRS=crypto ssl

ifdef CONFIG_DYN_LINK
  LIB_EXT=so
else
  LIB_EXT=a
endif

ifdef CONFIG_RG_OPENSSL
  SUBDIRS+=ssl
  SHLIBDIRS+=ssl
  EXPORT_LIBS+=libssl.$(LIB_EXT) liblocal_ssl.a
endif

EXPORT_HEADERS_DIR=openssl
EXPORT_HEADERS+=e_os.h e_os2.h apps/apps.h__apps.h

CLEAN+=shlib/*.o fluff *.map rehash.time testlog make.log cctest cctest.c \
  speed.* *.map *.so .pure

LAST_TARGET+=add_locals

include $(RGMK)

libcrypto$(SHLIB_EXT): libcrypto.a
	@if [ "$(SHLIB_TARGET)" != "" ]; then \
		$(MAKE) SHLIBDIRS=crypto build-shared; \
	else \
		echo "There's no support for shared libraries on this platform" >&2; \
	fi
libssl$(SHLIB_EXT): libcrypto$(SHLIB_EXT) libssl.a
	@if [ "$(SHLIB_TARGET)" != "" ]; then \
		$(MAKE) SHLIBDIRS=ssl SHLIBDEPS='-lcrypto' build-shared; \
	else \
		echo "There's no support for shared libraries on this platform" >&2; \
	fi

link-shared:
	@if [ -n "$(SHARED_LIBS_LINK_EXTS)" ]; then \
		tmp="$(SHARED_LIBS_LINK_EXTS)"; \
		for i in $(SHLIBDIRS); do \
			prev=lib$$i$(SHLIB_EXT); \
			for j in $${tmp:-x}; do \
				( set -x; ln -f -s $$prev lib$$i$$j ); \
				prev=lib$$i$$j; \
			done; \
		done; \
	fi

build-shared: clean-shared do_$(SHLIB_TARGET) link-shared

do_bsd-gcc-shared: do_gnu-shared
do_linux-shared: do_gnu-shared
do_gnu-shared:
	libs='-L. ${SHLIBDEPS}'; for i in ${SHLIBDIRS}; do \
	( set -x; ${CC} ${SHARED_LDFLAGS} \
		-shared -o lib$$i.so.${SHLIB_MAJOR}.${SHLIB_MINOR} \
		-Wl,-soname=lib$$i.so.${SHLIB_MAJOR}.${SHLIB_MINOR} \
		-Wl,-Bsymbolic \
		-Wl,--whole-archive lib$$i.a \
		-Wl,--no-whole-archive $$libs ${EX_LIBS} -lc ) || exit 1; \
	libs="$$libs -l$$i"; \
	done

DETECT_GNU_LD=${CC} -v 2>&1 | grep '^gcc' >/dev/null 2>&1 && \
	collect2=`gcc -print-prog-name=collect2 2>&1` && \
	[ -n "$$collect2" ] && \
	my_ld=`$$collect2 --help 2>&1 | grep Usage: | sed 's/^Usage: *\([^ ][^ ]*\).*/\1/'` && \
	[ -n "$$my_ld" ] && \
	$$my_ld -v 2>&1 | grep 'GNU ld' >/dev/null 2>&1

# For Darwin AKA Mac OS/X (dyld)
do_darwin-shared: 
	libs='-L. ${SHLIBDEPS}'; for i in ${SHLIBDIRS}; do \
	( set -x ; ${CC} --verbose -dynamiclib -o lib$$i${SHLIB_EXT} \
		lib$$i.a $$libs -all_load -current_version ${SHLIB_MAJOR}.${SHLIB_MINOR} \
		-compatibility_version ${SHLIB_MAJOR}.`echo ${SHLIB_MINOR} | cut -d. -f1` \
		-install_name ${INSTALLTOP}/lib/lib$$i${SHLIB_EXT} ) || exit 1; \
	libs="$$libs -l`basename $$i${SHLIB_EXT} .dylib`"; \
	echo "" ; \
	done

do_cygwin-shared:
	libs='-L. ${SHLIBDEPS}'; for i in ${SHLIBDIRS}; do \
	( set -x; ${CC}  -shared -o cyg$$i.dll \
		-Wl,-Bsymbolic \
		-Wl,--whole-archive lib$$i.a \
		-Wl,--out-implib,lib$$i.dll.a \
		-Wl,--no-whole-archive $$libs ) || exit 1; \
	libs="$$libs -l$$i"; \
	done

# This assumes that GNU utilities are *not* used
do_alpha-osf1-shared:
	if ${DETECT_GNU_LD}; then \
		$(MAKE) do_gnu-shared; \
	else \
		libs='-L. ${SHLIBDEPS}'; for i in ${SHLIBDIRS}; do \
		( set -x; ${CC} ${SHARED_LDFLAGS} \
			-shared -o lib$$i.so \
			-set_version "${SHLIB_VERSION_HISTORY}${SHLIB_VERSION_NUMBER}" \
			-all lib$$i.a -none $$libs ${EX_LIBS} -lc ) || exit 1; \
		libs="$$libs -l$$i"; \
		done; \
	fi

# This assumes that GNU utilities are *not* used
# The difference between alpha-osf1-shared and tru64-shared is the `-msym'
# option passed to the linker.
do_tru64-shared:
	if ${DETECT_GNU_LD}; then \
		$(MAKE) do_gnu-shared; \
	else \
		libs='-L. ${SHLIBDEPS}'; for i in ${SHLIBDIRS}; do \
		( set -x; ${CC} ${SHARED_LDFLAGS} \
			-shared -msym -o lib$$i.so \
			-set_version "${SHLIB_VERSION_HISTORY}${SHLIB_VERSION_NUMBER}" \
			-all lib$$i.a -none $$libs ${EX_LIBS} -lc ) || exit 1; \
		libs="$$libs -l$$i"; \
		done; \
	fi

# This assumes that GNU utilities are *not* used
# The difference between tru64-shared and tru64-shared-rpath is the
# -rpath ${INSTALLTOP}/lib passed to the linker.
do_tru64-shared-rpath:
	if ${DETECT_GNU_LD}; then \
		$(MAKE) do_gnu-shared; \
	else \
		libs='-L. ${SHLIBDEPS}'; for i in ${SHLIBDIRS}; do \
		( set -x; ${CC} ${SHARED_LDFLAGS} \
			-shared -msym -o lib$$i.so \
			-rpath  ${INSTALLTOP}/lib \
			-set_version "${SHLIB_VERSION_HISTORY}${SHLIB_VERSION_NUMBER}" \
			-all lib$$i.a -none $$libs ${EX_LIBS} -lc ) || exit 1; \
		libs="$$libs -l$$i"; \
		done; \
	fi


# This assumes that GNU utilities are *not* used
do_solaris-shared:
	if ${DETECT_GNU_LD}; then \
		$(MAKE) do_gnu-shared; \
	else \
		libs='-L. ${SHLIBDEPS}'; for i in ${SHLIBDIRS}; do \
		( PATH=/usr/ccs/bin:$$PATH ; export PATH; \
		  set -x; ${CC} ${SHARED_LDFLAGS} \
			-G -o lib$$i.so.${SHLIB_MAJOR}.${SHLIB_MINOR} \
			-h lib$$i.so.${SHLIB_MAJOR}.${SHLIB_MINOR} \
			-z allextract lib$$i.a $$libs ${EX_LIBS} -lc ) || exit 1; \
		libs="$$libs -l$$i"; \
		done; \
	fi

# OpenServer 5 native compilers used
do_svr3-shared:
	if ${DETECT_GNU_LD}; then \
		$(MAKE) do_gnu-shared; \
	else \
		libs='-L. ${SHLIBDEPS}'; for i in ${SHLIBDIRS}; do \
		( PATH=/usr/ccs/bin:$$PATH ; export PATH; \
		  find . -name "*.o" -print > allobjs ; \
		  OBJS= ; export OBJS ; \
		  for obj in `ar t lib$$i.a` ; do \
		    OBJS="$${OBJS} `grep $$obj allobjs`" ; \
		  done ; \
		  set -x; ${CC}  -G -o lib$$i.so.${SHLIB_MAJOR}.${SHLIB_MINOR} \
			-h lib$$i.so.${SHLIB_MAJOR}.${SHLIB_MINOR} \
			$${OBJS} $$libs ${EX_LIBS} ) || exit 1; \
		libs="$$libs -l$$i"; \
		done; \
	fi

# UnixWare 7 and OpenUNIX 8 native compilers used
do_svr5-shared:
	if ${DETECT_GNU_LD}; then \
		$(MAKE) do_gnu-shared; \
	else \
		libs='-L. ${SHLIBDEPS}'; for i in ${SHLIBDIRS}; do \
		( PATH=/usr/ccs/bin:$$PATH ; export PATH; \
		  find . -name "*.o" -print > allobjs ; \
		  OBJS= ; export OBJS ; \
		  for obj in `ar t lib$$i.a` ; do \
		    OBJS="$${OBJS} `grep $$obj allobjs`" ; \
		  done ; \
		  set -x; ${CC} ${SHARED_LDFLAGS} \
			-G -o lib$$i.so.${SHLIB_MAJOR}.${SHLIB_MINOR} \
			-h lib$$i.so.${SHLIB_MAJOR}.${SHLIB_MINOR} \
			$${OBJS} $$libs ${EX_LIBS} ) || exit 1; \
		libs="$$libs -l$$i"; \
		done; \
	fi

# This assumes that GNU utilities are *not* used
do_irix-shared:
	if ${DETECT_GNU_LD}; then \
		$(MAKE) do_gnu-shared; \
	else \
		libs='-L. ${SHLIBDEPS}'; for i in ${SHLIBDIRS}; do \
		( set -x; ${CC} ${SHARED_LDFLAGS} \
			-shared -o lib$$i.so.${SHLIB_MAJOR}.${SHLIB_MINOR} \
			-Wl,-soname,lib$$i.so.${SHLIB_MAJOR}.${SHLIB_MINOR} \
			-all lib$$i.a $$libs ${EX_LIBS} -lc) || exit 1; \
		libs="$$libs -l$$i"; \
		done; \
	fi

# This assumes that GNU utilities are *not* used
do_hpux-shared:
	libs='-L. ${SHLIBDEPS}'; for i in ${SHLIBDIRS}; do \
	( set -x; /usr/ccs/bin/ld ${SHARED_LDFLAGS} \
		+vnocompatwarnings \
		-b -z -o lib$$i.sl.${SHLIB_MAJOR}.${SHLIB_MINOR} \
		+h lib$$i.sl.${SHLIB_MAJOR}.${SHLIB_MINOR} \
		-Fl lib$$i.a $$libs ${EX_LIBS} -lc ) || exit 1; \
	libs="$$libs -L. -l$$i"; \
	done

# This assumes that GNU utilities are *not* used
do_hpux64-shared:
	libs='-L. ${SHLIBDEPS}'; for i in ${SHLIBDIRS}; do \
	( set -x; /usr/ccs/bin/ld ${SHARED_LDFLAGS} \
		-b -z -o lib$$i.sl.${SHLIB_MAJOR}.${SHLIB_MINOR} \
		+h lib$$i.sl.${SHLIB_MAJOR}.${SHLIB_MINOR} \
		+forceload lib$$i.a $$libs ${EX_LIBS} -lc ) || exit 1; \
	libs="$$libs -L. -l$$i"; \
	done

# The following method is said to work on all platforms.  Tests will
# determine if that's how it's gong to be used.
# This assumes that for all but GNU systems, GNU utilities are *not* used.
# ALLSYMSFLAGS would be:
#  GNU systems: --whole-archive
#  Tru64 Unix:  -all
#  Solaris:     -z allextract
#  Irix:        -all
#  HP/UX-32bit: -Fl
#  HP/UX-64bit: +forceload
#  AIX:		-bnogc
# SHAREDFLAGS would be:
#  GNU systems: -shared -Wl,-soname=lib$$i.so.${SHLIB_MAJOR}.${SHLIB_MINOR}
#  Tru64 Unix:  -shared \
#		-set_version "${SHLIB_VERSION_HISTORY}${SHLIB_VERSION_NUMBER}"
#  Solaris:     -G -h lib$$i.so.${SHLIB_MAJOR}.${SHLIB_MINOR}
#  Irix:        -shared -Wl,-soname,lib$$i.so.${SHLIB_MAJOR}.${SHLIB_MINOR}
#  HP/UX-32bit: +vnocompatwarnings -b -z +s \
#		+h lib$$i.sl.${SHLIB_MAJOR}.${SHLIB_MINOR}
#  HP/UX-64bit: -b -z +h lib$$i.sl.${SHLIB_MAJOR}.${SHLIB_MINOR}
#  AIX:		-G -bE:lib$$i.exp -bM:SRE
# SHAREDCMD would be:
#  GNU systems: $(CC)
#  Tru64 Unix:  $(CC)
#  Solaris:     $(CC)
#  Irix:        $(CC)
#  HP/UX-32bit: /usr/ccs/bin/ld
#  HP/UX-64bit: /usr/ccs/bin/ld
#  AIX:		$(CC)
ALLSYMSFLAG=-bnogc
SHAREDFLAGS=${SHARED_LDFLAGS} -G -bE:lib$$i.exp -bM:SRE
SHAREDCMD=$(CC)
do_aix-shared:
	libs='-L. ${SHLIBDEPS}'; for i in ${SHLIBDIRS}; do \
	( set -x; \
	  ld -r -o $$i.o $(ALLSYMSFLAG) lib$$i.a && \
	  ( nm -Pg lib$$i.o | grep ' [BD] ' | cut -f1 -d' ' > lib$$i.exp; \
	    $(SHAREDCMD) $(SHAREDFLAG) -o lib$$i.so lib$$i.o \
		$$libs ${EX_LIBS} ) ) \
	|| exit 1; \
	libs="$$libs -l$$i"; \
	done

Makefile.ssl: Makefile.org
	@echo "Makefile.ssl is older than Makefile.org."
	@echo "Reconfigure the source tree (via './config' or 'perl Configure'), please."
	@false

rehash: rehash.time
rehash.time: certs
	@(OPENSSL="`pwd`/apps/openssl"; export OPENSSL; $(PERL) tools/c_rehash certs)
	touch rehash.time

test:   tests

tests: rehash
	@(cd test && echo "testing..." && $(MAKE) tests );
	@apps/openssl version -a

report:
	@$(PERL) util/selftest.pl

# XXX: once dependence flags areworking, must remove this from remark and fix as
# it should be, according to DEPFLAG
#depend:
#	@for i in $(DIRS) ;\
#	do \
#	if [ -d "$$i" ]; then \
#		(cd $$i && echo "making dependencies $$i..." && \
#		$(MAKE) SDIRS='${SDIRS}' DEPFLAG='${DEPFLAG}' depend ) || exit 1; \
#	fi; \
#	done;

errors:
	$(PERL) util/mkerr.pl -recurse -write

stacks:
	$(PERL) util/mkstack.pl -write

util/libeay.num::
	$(PERL) util/mkdef.pl crypto update

util/ssleay.num::
	$(PERL) util/mkdef.pl ssl update

TABLE: Configure
	(echo 'Output of `Configure TABLE'"':"; \
	$(PERL) Configure TABLE) > TABLE
