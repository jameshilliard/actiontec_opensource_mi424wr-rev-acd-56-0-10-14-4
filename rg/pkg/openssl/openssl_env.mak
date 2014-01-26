OPENSSLDIR=/usr/local/ssl
INSTALLTOP=/usr/local/ssl

EXPORT_HEADERS_DIR=openssl

CFLAG=-DNO_ASM -DNO_BF -DNO_CAST -DNO_MDC2 -DNO_RC5 \
  -DNO_IDEA -DTERMIO

ifeq ($(TARGET_ENDIANESS),BIG)
  CFLAGS+=-DB_ENDIAN
else
  CFLAGS+=-DL_ENDIAN
endif

ifdef CONFIG_RG_OS_LINUX
  PLATFORM=linux-elf
  ifdef CONFIG_DYN_LINK
    CFLAGS+=-DDSO_DLFCN -DHAVE_DLFCN_H 
  endif
endif
ifdef CONFIG_RG_OS_VXWORKS
  PLATFORM=vxworks
  CFLAGS+=-DVXWORKS
endif

PERL=perl
EX_LIBS=
PROCESSOR= 

BN_ASM=bn_asm.o
DES_ENC=des_enc.o fcrypt_b.o
BF_ENC=bf_enc.o
CAST_ENC=c_enc.o
RC4_ENC=rc4_enc.o
RC5_ENC=rc5_enc.o
MD5_ASM_OBJ=
SHA1_ASM_OBJ=
RMD160_ASM_OBJ=

# XXX OpenSSL's headers are not strict ANSI (do not have full prototypes).
# Therefore we must disable the strict prototype checking only for this package
CFLAGS := $(filter-out -Wstrict-prototypes,$(CFLAGS))

CD_EXPORTED_FILES=$(EXHEADER) $(TEST) $(APPS)

SSLMK=$(TOP)/rules.mak
