#
# For a description of the syntax of this configuration file,
# see extra/config/Kconfig-language.txt
#

mainmenu "uClibc C Library Configuration"

menu "Target Architecture Features and Options"

config HAVE_ELF
	bool
	default y

config ARCH_CFLAGS
	string

config ARCH_LDFLAGS
	string

config LIBGCC_CFLAGS
	string

config HAVE_DOT_HIDDEN
        bool
	default y

config UCLIBC_COMPLETELY_PIC
        bool
	default y

choice
	prompt "Target Processor Type"
	default CONFIG_SH4
	help
	  This is the processor type of your CPU. This information is used for
	  optimizing purposes, as well as to determine if your CPU has an MMU,
	  an FPU, etc.  If you pick the wrong CPU type, there is no guarantee
	  that uClibc will work at all....

	  Here are the available choices:
	  - "SH2" Hitachi SH2
	  - "SH3" Hitachi SH3
	  - "SH4" Hitachi SH4
	  - "SH5" Hitachi SH5

config CONFIG_SH2
	bool "SH2"

config CONFIG_SH3
	bool "SH3"

config CONFIG_SH4
	bool "SH4"

config CONFIG_SH5
	bool "SH5"

endchoice

choice
	prompt "Target Processor Endianness"
	default ARCH_LITTLE_ENDIAN
	help
	  This is the endianness you wish to build use.  Choose either Big
	  Endian, or Little Endian.

config ARCH_LITTLE_ENDIAN
	bool "Little Endian"

config ARCH_BIG_ENDIAN
	bool "Big Endian"

endchoice


config ARCH_HAS_NO_MMU
	bool
	default y if CONFIG_SH2

config ARCH_HAS_NO_FPU
       bool
       default y if CONFIG_SH2 || CONFIG_SH3

source "extra/Configs/Config.in.arch"

endmenu

source "extra/Configs/Config.in"


