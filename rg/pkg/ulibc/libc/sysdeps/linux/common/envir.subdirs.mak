ifeq ($(filter mips powerpc,$(LIBC_ARCH)),$(LIBC_ARCH))
  # Unresolved warnings:
  # "use of memory input without lvalue in asm operand 7 is deprecated"
  WARN2ERR=n
endif
