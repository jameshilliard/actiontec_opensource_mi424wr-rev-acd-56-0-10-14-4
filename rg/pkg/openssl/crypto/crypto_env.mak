OPENSSL_DIR=$(RGSRC)/pkg/openssl
include $(OPENSSL_DIR)/openssl_env.mak

INCLUDES=-I$(call OS_PATH,$(OPENSSL_DIR)) \
	 -I$(call OS_PATH,$(OPENSSL_DIR)/include) \
	 -I$(call OS_PATH,$(OPENSSL_DIR)/crypto)
  
CFLAGS+= $(INCLUDES) $(CFLAG)
LOCAL_CFLAGS+=$(INCLUDES) $(CFLAG)

CRYPTO_RG_LIB=libcrypto_rg.a
CRYPTO_LIB=libcrypto.a

# Always add to libcrypto
A_TARGET+=$(BUILDDIR)/pkg/openssl/crypto/$(CRYPTO_LIB)
ifdef CREATE_LIB
  A_TARGET+=$(BUILDDIR)/pkg/openssl/crypto/$(CRYPTO_RG_LIB)
endif

CREATE_LOCAL=$(A_TARGET)

