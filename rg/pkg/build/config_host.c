/****************************************************************************
 *
 * rg/pkg/build/config_host.c
 * 
 * Copyright (C) Jungo LTD 2004
 * 
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General 
 * Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 * Developed by Jungo LTD.
 * Residential Gateway Software Division
 * www.jungo.com
 * info@jungo.com
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "str.h"

#include "config_opt.h"
#include "create_config.h"

/* List of CONFIGs:
 * TODO Move all CONFIGs to use a global char *
 */
static char *BUILDDIR, *RGSRC, *CCACHE, *TOOLS_PREFIX, *TOOLCHAIN_ROOT;

static void conf_ostype(void)
{
    char *ostype = NULL;
    int ret;

    /* Host OS: Set CONFIG_RG_HOST_xxx */
    ostype = sys_get(&ret, "echo $OSTYPE");
    if (ret)
	conf_err("$OSTYPE not defined\n");

    /* the gcc has built-in */
    if (!strcmp(ostype, "cygwin"))
    {
	token_set_y("CONFIG_RG_HOST_CYGWIN");
	token_set_y("CONFIG_RG_WINDOWS_HOST"); /* backwards compatibility */
    }
    else if (!strncmp(ostype, "linux", 5))
	token_set_y("CONFIG_RG_HOST_LINUX");
    else
	conf_err("OS '%s' not supported ($OSTYPE)\n", ostype);
    str_free(&ostype);
}

static void conf_os_path(void)
{
    /* OS-dependent directory name translation */
    if (token_get("CONFIG_WINDOWS_HOST"))
	token_set("OS_PATH", "\"$(shell cygpath -w $(1))\"");
    else
	token_set("OS_PATH", "$(1)");
}

static void conf_endian_flags(void)
{
    char *t;

    if (!(t = token_get_str("TARGET_ENDIANESS")))
	conf_err("Can't find TARGET_ENDIANESS");
    
    if (!strcmp(t,"BIG"))
    {
	token_set("ENDIAN_LDFLAGS", "-EB");
	if (token_get("CONFIG_CPU_XSCALE") || token_get("CONFIG_CPU_ARM926T"))
	    token_set("ENDIAN_CFLAGS", "-mbig-endian");
	else if (token_get("CONFIG_CPU_MIPS32"))
	{
	    if (token_get("CONFIG_RG_OS_VXWORKS"))
		token_set("ENDIAN_CFLAGS", "-EB");
	    else
		token_set("ENDIAN_CFLAGS", "-meb");
	}
	else
	    token_set("ENDIAN_CFLAGS", "-mbig-endian");
    }
    else if (!strcmp(t,"LITTLE"))
    {
	if (!token_get("CONFIG_M386"))
	    token_set("ENDIAN_LDFLAGS", "-EL");
	if (token_get("CONFIG_CPU_XSCALE") || token_get("CONFIG_CPU_ARM926T"))
	    token_set("ENDIAN_CFLAGS", "-mlittle-endian");
	else if (token_get("CONFIG_CPU_MIPS32"))
	{
	    if (token_get("CONFIG_RG_OS_VXWORKS"))
		token_set("ENDIAN_CFLAGS", "-EL");
	    else
		token_set("ENDIAN_CFLAGS", "-mel");
	}
	else if (!token_get("CONFIG_M386"))
	    token_set("ENDIAN_CFLAGS", "-mlittle-endian");
    }
    else
	conf_err("Illegal TARGET_ENDIANESS: %s", t);
}

static void conf_toolchain(void)
{
    /* Toolchain */
    str_catprintf(&TOOLCHAIN_ROOT, "/usr/local/openrg");
    token_set("TOOLCHAIN_ROOT", "%s", TOOLCHAIN_ROOT);
    token_set("I386_TARGET_MACHINE", "i386-jungo-linux-gnu");
    token_set("I386_TOOLS_PREFIX", "%s/%s", TOOLCHAIN_ROOT,
	token_get_str("I386_TARGET_MACHINE"));
    
    /* can make this auto-detect in the future */
    if (!token_get_str("RG_GLIBC_VERSION"))
	token_set("RG_GLIBC_VERSION", "2.3.2");
}

static void conf_target_machine(void)
{
    if (!token_get_str("ARCH"))
    {
	printf("conf_target_machine: Error: ARCH not defined.\n");
	exit(1);
    }

    if (!strcmp(token_get_str("ARCH"), "i386") ||
	!strcmp(token_get_str("ARCH"), "um"))
    {
	token_set("TARGET_MACHINE", token_get_str("I386_TARGET_MACHINE"));
    }
    if (token_get("CONFIG_BCM963XX") || token_get("CONFIG_INCAIP") ||
	token_get("CONFIG_INFINEON_AMAZON"))
    {
	token_set("TARGET_MACHINE", "mipseb-jungo-linux-gnu");
    }
    if (token_get("CONFIG_BCM947XX") || token_get("CONFIG_ADM5120_COMMON"))
	token_set("TARGET_MACHINE", "mipsel-jungo-linux-gnu");
    if (token_get("CONFIG_ARMNOMMU"))
    {
	if (token_get("CONFIG_RG_OS_LINUX"))
	    token_set("TARGET_MACHINE", "arm-jungo-linux-gnu");
	if (token_get("CONFIG_RG_OS_VXWORKS"))
	    token_set("TARGET_MACHINE", "arm-little-coff");
    }
    if (token_get("CONFIG_SL2312_COMMON"))
    {
	/* XXX TODO Change 'arm' into $RG_CPU */
	token_set("TARGET_MACHINE", "arm_920t_le"); 
    }
    if (token_get("CONFIG_STRONGARM"))
    {
	if (!token_get("CONFIG_RG_OLD_XSCALE_TOOLCHAIN"))
	    token_set("TARGET_MACHINE", "armv5b-jungo-linux-gnu");
	else if (!strcmp(token_getz("TARGET_ENDIANESS"), "LITTLE"))
	    conf_err("No little endian strong arm support yet");
	else
	{
	    /* XXX TODO Change 'armv4b' into $RG_CPU */
	    token_set("TARGET_MACHINE", "armv4b-hardhat-linux");
	}
    }
    if (token_get("CONFIG_CX8620X")) 
	token_set("TARGET_MACHINE", "armv5l-jungo-linux-gnu");
    if (token_get("CONFIG_KS8695")) 
	token_set("TARGET_MACHINE", "armv5l-jungo-linux-gnu");
    if (token_get("CONFIG_RG_OS_VXWORKS"))
    {
	if (token_get("CONFIG_I386_BOCHS"))
	    token_set("TARGET_MACHINE", "i386-aout");
	if (token_get("CONFIG_ATHEROS_AR531X_MIPS"))
	    token_set("TARGET_MACHINE", "mips-elf");
	if (token_get("CONFIG_TI_404_MIPS"))
	    token_set("TARGET_MACHINE", "mips-elf");
    }
    if (token_get("CONFIG_PPC")) 
	token_set("TARGET_MACHINE", "powerpc-jungo-linux-gnu");

    /* XXX Maybe we should remove this? */
    token_set("CROSS_COMPILE", "%s-", token_get_str("TARGET_MACHINE"));
}

static void conf_tools_prefix(void)
{
    if (token_get("CONFIG_ARMNOMMU") && token_get("CONFIG_RG_OS_VXWORKS"))
    {
	str_catprintf(&TOOLS_PREFIX, "%s/vx_%s", TOOLCHAIN_ROOT,
	    token_get_str("ARCH"));
    }

    if (token_get("CONFIG_STRONGARM") && 
	token_get("CONFIG_RG_OLD_XSCALE_TOOLCHAIN"))
    {
	str_catprintf(&TOOLS_PREFIX, "%s/%s%s/", TOOLCHAIN_ROOT,
	    token_get_str("ARCH"), token_get_str("ENDIANESS_SUFFIX"));
    }

    if (token_get("CONFIG_TI_404_MIPS") && token_get("CONFIG_RG_OS_VXWORKS"))
    {
	str_catprintf(&TOOLS_PREFIX, "%s/vx_%s_323", TOOLCHAIN_ROOT,
	    token_get_str("ARCH"));
    }

    if (token_get("CONFIG_ATHEROS_AR531X_MIPS") && 
	token_get("CONFIG_RG_OS_VXWORKS"))
    {
	str_catprintf(&TOOLS_PREFIX, "%s/vx_%s_323", TOOLCHAIN_ROOT,
	    token_get_str("ARCH"));
    }

    if (token_get("CONFIG_I386_BOCHS") && token_get("CONFIG_RG_OS_VXWORKS"))
    {
	str_catprintf(&TOOLS_PREFIX, "%s/vx_%s", TOOLCHAIN_ROOT,
	    token_get_str("ARCH"));
    }

#if 0
    if (token_get("CONFIG_INFINEON_AMAZON"))
    {
	str_catprintf(&TOOLS_PREFIX, "%s/%s-eb-3.3.4", TOOLCHAIN_ROOT,
	    token_get_str("TARGET_MACHINE"));
    }
#endif

    if (!TOOLS_PREFIX)
    {
	str_catprintf(&TOOLS_PREFIX, "%s/%s", TOOLCHAIN_ROOT, 
	    token_get_str("TARGET_MACHINE"));
    }

    token_set("TOOLS_PREFIX", "%s", TOOLS_PREFIX);
}

static void conf_ulibc_in_tooldchain(void)
{
    struct stat st;
    char *u = NULL;
    
    if (!token_get("CONFIG_ULIBC"))
	return;

    printf("Detecting precompiled ulibc: ");
    if (!strcmp(token_getz("CONFIG_RG_ULIBC_PRECOMPILED"), "n"))
    {
	printf("disabled\n");
	goto Exit; /* User chose not to use ulibc from tooldchain */
    }

    str_printf(&u, "%s/ulibc", TOOLS_PREFIX);
    if (lstat(u, &st))
    {
	if (!strcmp(token_getz("CONFIG_RG_ULIBC_PRECOMPILED"), "y"))
	{
	    conf_err("Can't find %s:\nEither insatll or "
		"remove CONFIG_RG_ULIBC_PRECOMPILED=y from command line\n", u);
	}
	printf("not found (searched %s)\n", u);
	goto Exit;
    }

    token_set_y("ULIBC_IN_TOOLCHAIN");
    printf("found %s (use CONFIG_RG_ULIBC_PRECOMPILED=n to disable)\n", u);
Exit:
    str_free(&u);
}

static void conf_glibc_in_tooldchain(void)
{
    struct stat st;
    char *s = NULL;

    if (!token_get("CONFIG_GLIBC"))
	return;

    printf("Detecting precompiled glibc: ");
    if (!strcmp(token_getz("CONFIG_RG_GLIBC_PRECOMPILED"), "n"))
    {
	printf("disabled\n");
	goto Exit; /* User chose not to use glibc from tooldchain */
    }

    str_printf(&s, "%s/%s/lib/libc-%s.so", TOOLS_PREFIX,
	token_get_str("TARGET_MACHINE"), token_get_str("RG_GLIBC_VERSION"));
    if (lstat(s, &st))
    {
	if (!strcmp(token_getz("CONFIG_RG_GLIBC_PRECOMPILED"), "y"))
	{
	    conf_err("Can't find %s:\nEither insatll or "
		"remove CONFIG_RG_GLIBC_PRECOMPILED=y from command line\n", s);
	}
	printf("not found (searched %s)\n", s);
	goto Exit;
    }

    token_set_y("GLIBC_IN_TOOLCHAIN");
    printf("found %s (use CONFIG_RG_GLIBC_PRECOMPILED=n to disable)\n", s);
Exit:
    if (!token_get("GLIBC_IN_TOOLCHAIN"))
	conf_err("Only pre-compiled GLIBC is currently supported");
    str_free(&s);
}

static void conf_ccache(void)
{
    char *ccache = NULL;
    char *s = NULL;
    int ret;

    str_alloc(&CCACHE);
    str_cpy(&ccache, token_getz("CONFIG_RG_CCACHE"));
    printf("Detecting ccache: ");
    if (!strcmp(ccache, "n"))
    {
	printf("disabled\n");
	goto Exit;
    }
    if (!*ccache || !strcmp(ccache, "y"))
    {
	/* autodetect ccache */
        str_trim(str_use(&ccache, sys_get(&ret, "which ccache")));
	if (ret || str_isempty(ccache))
	{
	    printf("not found\n");
	    goto Exit;
	}
    }
    /* check ccache exists */
    str_use(&s, sys_get(NULL, ccache));
    if (!strstr(s, "ccache"))
	conf_err("failed executing ccache (%s)", ccache);
    
    str_printf(&CCACHE, "%s ", ccache);
    printf("found %s (use CONFIG_RG_CCACHE=n to disable)\n",
	CCACHE);
Exit:
    str_free(&s);
    str_free(&ccache);
}

static void conf_cc(void)
{
    /* CC */
    if (token_get("CONFIG_ULIBC"))
    {
	if (token_get("ULIBC_IN_TOOLCHAIN"))
	{
	    token_set("CC", "%s/ulibc/%s-uclibc-gcc", TOOLS_PREFIX,
		token_get_str("LIBC_ARCH"));
	}
	else
	{
	    token_set("CC", "%s/pkg/ulibc/extra/gcc-uClibc/%s-uclibc-gcc",
		BUILDDIR, token_get_str("LIBC_ARCH"));
	}
    }

    if (token_get("CONFIG_GLIBC"))
	token_set("CC", "%s/pkg/build/rg_gcc", BUILDDIR);
    
    if (token_get("CONFIG_RG_OS_VXWORKS"))
    {
	if (token_get("CONFIG_WINDOWS_HOST"))
	{
	    token_set("CC", "%s%s/bin/cc%s", CCACHE, TOOLS_PREFIX,
		token_get_str("ARCH"));
	}
	else
	{
	    token_set("CC", "%s%s/bin/%s-gcc", CCACHE, TOOLS_PREFIX,
		token_get_str("TARGET_MACHINE"));
	}
    }

    /* CXX */
    token_set("CPP", "%s -E", token_get_str("CC"));
    token_set("CXX", "%s%s/bin/%s-g++", CCACHE, token_get_str("TOOLS_PREFIX"), 
	token_get_str("TARGET_MACHINE"));
}

static void conf_cc_for_build(void)
{
    token_set("CC_FOR_BUILD", "%s%s/bin/i386-jungo-linux-gnu-gcc "
	"-DCC_FOR_BUILD -I%s/pkg/include -I%s/pkg/include -DCONFIG_HAS_MMU",
	CCACHE, token_get_str("I386_TOOLS_PREFIX"), RGSRC, BUILDDIR);
    
    token_set("CXX_FOR_BUILD", "%s%s/bin/i386-jungo-linux-gnu-g++ "
	"-DCC_FOR_BUILD -I%s/pkg/include -I%s/pkg/include -DCONFIG_HAS_MMU",
	CCACHE, token_get_str("I386_TOOLS_PREFIX"), RGSRC, BUILDDIR);
}

static void conf_target_cc(void)
{
    token_set("TARGET_CC", "%s%s/bin/%s-gcc", CCACHE, TOOLS_PREFIX,
	token_get_str("TARGET_MACHINE"));

    token_set("TARGET_CXX", "%s%s/bin/%s-g++", CCACHE, TOOLS_PREFIX,
	token_get_str("TARGET_MACHINE"));

    token_set("REAL_GCC", "%s", token_get_str("TARGET_CC"));
}

static void conf_gcc_version(void)
{
    char *bn, *ver = sys_get(NULL, "%s -dumpversion",
	token_get_str("TARGET_CC"));

    if (!ver)
	conf_err("Cannot configure GCC_VERSION");
    
    if ((bn = strchr(ver, '\n')))
	*bn = '\0';
    
    token_set("GCC_VERSION", ver);

    if (token_get("CONFIG_ULIBC"))
	token_set("RG_LIB_VERSION", "ulibc");
    else if (token_get("CONFIG_GLIBC"))
    {
	token_set("RG_LIB_VERSION", "glibc");
	token_set("LIBC_DIR_PREFIX", "%s/lib/gcc-lib/%s/%s", TOOLS_PREFIX,
	    token_get_str("TARGET_MACHINE"), ver);
    }
    str_free(&ver);
}

static void conf_host_tools(void)
{
    char postfix[256] = "", prefix[256];

    if (token_get("CONFIG_WINDOWS_HOST"))
    {
	sprintf(postfix, "i386");
	sprintf(prefix, "%s/bin/", TOOLS_PREFIX);
    }
    else
    {
	sprintf(prefix, "%s/bin/i386-jungo-linux-gnu-",
	    token_get_str("I386_TOOLS_PREFIX"));
    }

    token_set("HOST_AR", "%sar%s", prefix, postfix);
    token_set("HOST_LD", "%sld%s", prefix, postfix);
    token_set("HOST_AS", "%sas%s", prefix, postfix);
    token_set("HOST_NM", "%snm%s", prefix, postfix);
    token_set("HOST_RANLIB", "%sranlib%s", prefix, postfix);
    token_set("HOST_STRIP", "%sstrip%s", prefix, postfix);
    token_set("HOST_SIZE", "%ssize%s", prefix, postfix);
    token_set("HOST_OBJCOPY", "%sobjcopy%s", prefix, postfix);
    token_set("HOST_OBJDUMP", "%sobjdump%s", prefix, postfix);
}

static void conf_tools(void)
{
    char postfix[256] = "", prefix[256];

    if (token_get("CONFIG_WINDOWS_HOST"))
    {
	sprintf(postfix, "%s", token_get_str("ARCH"));
	sprintf(prefix, "%s/bin/", TOOLS_PREFIX);
    }
    else
    {
	sprintf(prefix, "%s/bin/%s-", TOOLS_PREFIX,
	    token_get_str("TARGET_MACHINE"));
    }

    token_set("AR", "%sar%s", prefix, postfix);
    token_set("LD", "%sld%s", prefix, postfix);
    token_set("AS", "%sas%s", prefix, postfix);
    token_set("NM", "%snm%s", prefix, postfix);
    token_set("RANLIB", "%sranlib%s", prefix, postfix);
    token_set("STRIP", "%sstrip%s", prefix, postfix);
    token_set("SIZE", "%ssize%s", prefix, postfix);
    token_set("OBJCOPY", "%sobjcopy%s", prefix, postfix);
    token_set("OBJDUMP", "%sobjdump%s", prefix, postfix);
}

static void conf_os_target(void)
{
    token_set("PROD_LINUX_TARGET", "bzImage.initrd");

    if (token_get("CONFIG_RG_OS_LINUX"))
	token_set("OS_TARGET", "%s", token_get_str("PROD_LINUX_TARGET"));

    if (!token_get("CONFIG_RG_OS_VXWORKS"))
	return;

    if (token_get("CONFIG_TI_404_MIPS"))
	token_set("OS_TARGET", "rgcm.img");
    else if (token_get("CONFIG_ATHEROS_AR531X_MIPS"))
	token_set("OS_TARGET", "vxWorksCompressed");
    else
	token_set("OS_TARGET", "vxWorks.st");
}

/* General configuration */
static void conf_host_general(void)
{
    char *bison = sys_get(NULL, "bison --version");
    printf("Detecting bison: ");
    if (!bison || !*bison)
    {
	printf("Not found.\n");
	rg_error(LEXIT, "Please install bison.\n");
    }
    else
    {
	/* print only the first line of the 'bison --version' output */
	if (strchr(bison, '\n'))
	    *strchr(bison, '\n') = 0;
	printf("found %s.\n", bison);
    }
    token_set("YACC", "bison");
    token_set("YFLAGS", "-y");
    token_set("SPCC", "echo");
    token_set("HOSTCC", "gcc -DCONFIG_HAS_MMU");
    token_set("CPP_FOR_BUILD", "%s -E", token_get_str("CC_FOR_BUILD"));

    token_set("GENROMFS", "%s/pkg/genromfs/genromfs", BUILDDIR);

    if (token_get("CONFIG_RG_OS_LINUX"))
	token_set("AUTOCONF_H", "include/linux/autoconf.h");

    /* net-tools generic Makefile */
    token_set("NET_TOOLS_MAKE_FILE", "%s/pkg/include/net-tools-config.make",
	RGSRC);

    token_set("DYNAMIC_LINKER", "/lib/ld-linux.so.1");
    token_set("LZMAMK", "%s/pkg/lzma/SRC/lzma.mak", RGSRC);

    /* local unit-test spawner */
    token_set("UNITTEST_SPAWNER", "%s/pkg/util/jspawn -s 20", BUILDDIR);

    str_free(&bison);
}

static void conf_target_general(void)
{
    token_set("RG_BIN", "%s/pkg/build/bin/", BUILDDIR);
    token_set("RG_INCLUDE", "%s/pkg/build/include/", BUILDDIR);
    token_set("RG_LIB", "%s/pkg/build/lib/", BUILDDIR);
    token_set("RG_INCLUDE_DIR", "%s/pkg/include", BUILDDIR);
    
    /* This subdir is used to debug modules */
    token_set("STATIC_MOD_DIR", "%s/os/kernel/modules_static/", BUILDDIR);

    token_set("RG_LIBPTHREAD_VERSION", "0.8");
    token_set("NEEDED_SYMBOLS", "%s/os/linux/needed_symbols.lst", BUILDDIR);
    
    /* XXX need to fix path using OS_PATH when working in windows */
    if (token_get("CONFIG_RG_OS_VXWORKS"))
	token_set("VX_INCLUDE_DIR", "%s/os/vx/target/h", BUILDDIR);

    if (token_get("CONFIG_ARMNOMMU") && token_get("CONFIG_RG_OS_LINUX"))
    {
	token_set("FLTHDR", "%s/bin/%s/-flthdr", TOOLS_PREFIX,
	    token_get_str("TARGET_MACHINE"));
    }

    if (token_get_str("LINUX_ARCH_PATH") && token_get("CONFIG_CRAMFS_FS"))
    {
	token_set("COMPRESSED_RAMDISK", "%s/ramdisk/ramdisk.gz",
	    token_get_str("LINUX_ARCH_PATH"));
	
	if (token_get_str("CONFIG_RG_INITFS_CRAMFS"))
	{
	    token_set("COMPRESSED_INIT_DISK", "%s/cramdisk/cramfs_init.img",
		token_get_str("LINUX_ARCH_PATH"));
	}
    }

    if (token_get_str("LINUX_ARCH_PATH"))
    {
	token_set("MODFS_DISK", "%s/ramdisk/mod.img",
	    token_get_str("LINUX_ARCH_PATH"));
    }

    /* Sometime we need a certain header file from the rg kernel. Therefore,
     * here we define a search path that will guarantee that gcc will first 
     * search the standard Linux libraries, and only if the file isn't found,
     * gcc will try rg kernel.
     */
    token_set("LOCAL_ADD_RG_KERNEL_FOR_TARGET", "-idirafter "
	"%s/os/linux/include", BUILDDIR);

    if (token_get("CONFIG_BINFMT_FLAT"))
    {
	token_set("LDFLAT", "%s/bin/ldelf2flt", TOOLS_PREFIX);
	token_set("BINFRMT_FILE", "target_binfmt_flat.mak");
    }
    else
	token_set("BINFRMT_FILE", "target_binfmt_elf.mak");

    token_set("PERM_STORAGE_FILES_DIR", "%s/pkg/flash/", RGSRC);

    /* PATH: Start with '.' for autogenerated executables */
    token_set("PATH", ".:%s:%s/bin", getenv("PATH"), TOOLS_PREFIX);
}

static void conf_openrg_img(void)
{
    char *img, *ldist = strdup(dist);

    /* OPENRG_IMG */
    if (token_get("CONFIG_RG_RGLOADER"))
	img = "rgloader.img";
    else if (token_get("CONFIG_LSP_DIST"))
	img = "lsp.img";
    else
	img = "openrg.img";

    token_set("OPENRG_IMG", "%s/os/%s", BUILDDIR, img);

    /* OPENRG_RMT_UPDATE_IMG */
    if (!token_get("CONFIG_LSP_DIST"))
	token_set("OPENRG_RMT_UPDATE_IMG", "%s/openrg.rmt", BUILDDIR);
    else
    {
	token_set("OPENRG_RMT_UPDATE_IMG", "%s/%s.rmt", *str_tolower(&ldist),
	    BUILDDIR);
    }

    token_set("OPENRG_PROD", "%s/os/openrg.prod", BUILDDIR);
    token_set("BOOTLDR_IMG", "%s/pkg/bootldr/arch/boot.img", BUILDDIR);
    token_set("FLASH_IMG", "%s/flash.img", "BUILDDIR");
}

static void conf_prod_kernel_image(void)
{
    char *filename = "vmlinux";
    
    if (!token_get("CONFIG_RG_OS_LINUX"))
	return;

    if (token_get("CONFIG_RG_UML"))
	filename = "openrg.uml";
    if (token_get("CONFIG_KS8695"))
	filename = "sl2312.elf";

    token_set("PROD_KERNEL_IMAGE", "%s/os/linux/%s", BUILDDIR, filename);
}

static void conf_trx(void)
{
    if (!token_get("CONFIG_BCM947XX") && !token_get("CONFIG_BCM963XX"))
	return;
    token_set("TRX", "%s/tools/trx", TOOLS_PREFIX);
}

static void conf_rg_cpu(void)
{
    if (token_get("CONFIG_SL2312_COMMON"))
	token_set("RG_CPU", "arm");
    
    if (token_get("CONFIG_STRONGARM") && 
	token_get("CONFIG_RG_OLD_XSCALE_TOOLCHAIN"))
    {
	token_set("RG_CPU", "armv4b");
    }

    if (token_get("CONFIG_TI_404_MIPS"))
	token_set("RG_CPU", "mips");

    if (token_get("CONFIG_ATHEROS_AR531X_MIPS"))
	token_set("RG_CPU", "mips");

    if (token_get("CONFIG_I386_BOCHS") && token_get("CONFIG_RG_OS_VXWORKS"))
	token_set("RG_CPU", "i386");

    if (token_get("CONFIG_PPC"))
	token_set("RG_CPU", "powerpc");

    /* XXX should this be here? Don't we have CONFIG_ARM??*/
    if (token_get("CONFIG_KS8695_COMMON"))
	token_set("RG_CPU", "arm");
}

static void conf_flash_img_offset(void)
{
    char *offset = NULL, *real_offset = NULL;

    if (token_get("CONFIG_SL2312_COMMON") || 
	token_get("CONFIG_KS8695_COMMON"))
    {
	offset = "0xc0000";
	real_offset = "0x0";
    }

    if (token_get("CONFIG_STRONGARM"))
    {
	offset = token_get("CONFIG_RG_BOOTLDR_REDBOOT") ? "0x40000" :
	    "0x100000";
	real_offset = "0x0";
    }

    if (token_get("CONFIG_CX8620X"))
    {
	offset = "0x4000000";
	real_offset = "0x0";
    }

    offset = offset?:"0x0";
    real_offset = real_offset?:offset;
    
    token_set("FLASH_IMG_OFFSET", "%s", offset);
    token_set("FLASH_IMG_REAL_OFFSET", "%s", real_offset);
}

static void conf_have_gc_sections(void)
{
    if (!token_get("CONFIG_MPC8272ADS") && !token_get("CONFIG_BCM963XX") &&
	!token_get("CONFIG_INCAIP") &&
	!token_get("CONFIG_ADM5120_COMMON") &&
	!token_get("CONFIG_CX8620X") &&
	!token_get("CONFIG_AMAZON") &&
	(!token_get("CONFIG_STRONGARM") ||
	token_get("CONFIG_RG_OLD_XSCALE_TOOLCHAIN")))
    {
	return;
    }

    token_set_y("HAVE_GC_SECTIONS");
}

static void conf_linux_arch_path(void)
{
    char *arch = NULL;

    if (token_get("CONFIG_BCM947XX"))
	arch = "mips";
    if (token_get("CONFIG_BCM963XX"))
	arch = "mips";
    if (token_get("CONFIG_INCAIP"))
	arch = "mips";
    if (token_get("CONFIG_ADM5120_COMMON"))
	arch = "mips";
    if (token_get("CONFIG_SL2312_COMMON"))
	arch = "arm";
    if (token_get("CONFIG_STRONGARM"))
	arch = "arm";
    if (token_get("CONFIG_CX8620X"))
	arch = "arm";
    if (token_get("CONFIG_KS8695"))
	arch = "arm";
    if (token_get("CONFIG_PPC"))
	arch = "ppc";
    if (token_get("CONFIG_INFINEON_AMAZON"))
	arch = "mips";

    if (!arch)
	return;

    token_set("LINUX_ARCH_PATH", "%s/os/linux/arch/%s", BUILDDIR, arch);
}

static void conf_compressed_disk(void)
{
    char *img;

    token_set("BOOTLDR_COMPRESSED_DISK",
	"%s/os/linux/arch/%s/boot/bootldr_ramdisk.gz", BUILDDIR,
	token_get_str("ARCH"));
    
    if (!token_get_str("LINUX_ARCH_PATH"))
    {
	token_set("COMPRESSED_DISK", "%s/pkg/build/ramdisk.gz", BUILDDIR);
	return;
    }

    img = token_get("CONFIG_CRAMFS_FS") ? "cramdisk/cramfs.img" :
	"ramdisk/ramdisk.gz" ;
	
    token_set("COMPRESSED_DISK", "%s/%s", token_get_str("LINUX_ARCH_PATH"),
	img);
}

static void conf_gcc_has_size_optimize(void)
{
    if (token_get("CONFIG_RG_OS_LINUX") && !token_get("CONFIG_IDT_COMMON"))
	token_set_y("GCC_HAS_SIZE_OPTIMIZE");
    
    if (token_get("CONFIG_RG_OS_VXWORKS") && token_get("CONFIG_TI_404_MIPS"))
	token_set_y("GCC_HAS_SIZE_OPTIMIZE");
    
    if (token_get("CONFIG_RG_OS_VXWORKS") && token_get("GCC_HAS_SIZE_OPTIMIZE"))
	token_set_y("GCC_HAS_SIZE_OPTIMIZE");
}

static void conf_rg_cflags(void)
{
    char *f = NULL;

    /* XXX need to fix path using OS_PATH when working in windows */
    str_printf(&f, "-I%s/pkg/include ", RGSRC);

    if (!token_get("CONFIG_RG_OS_VXWORKS"))
	str_catprintf(&f, "-I%s/pkg/build/include ", BUILDDIR);

    str_catprintf(&f, "-D_LIBC_REENTRANT -Wall -g ");

    if (token_get("CONFIG_RG_NO_OPT"))
	str_catprintf(&f, "-O0 ");
    else if (token_get("GCC_HAS_SIZE_OPTIMIZE"))
	str_catprintf(&f, "-Os ");
    else if (!token_get("CONFIG_TI_404_MIPS"))
	str_catprintf(&f, "-O2 ");

    /* For VxWorks do not use the sbss and sdata sections. */
    if (token_get("CONFIG_RG_OS_VXWORKS") && !token_get("CONFIG_I386_BOCHS"))
	str_catprintf(&f, "-G 0 ");
    
    token_set("RG_CFLAGS", "%s", f);
}

static void conf_arch_cflags(void)
{
    char *f = NULL;
    
    str_printf(&f, "%s ", token_getz("ENDIAN_CFLAGS"));

    if (token_get("CONFIG_BCM947XX"))
	str_catprintf(&f, "-mgp32 -mlong32 -march=4kc -mtune=4kc ");

    if (token_get("CONFIG_BCM963XX"))
	str_catprintf(&f, "-mgp32 -mlong32 -march=4kp -mtune=4kp ");

    if (token_get("CONFIG_INCAIP"))
	str_catprintf(&f, "-mgp32 -mlong32 -march=4kc -mtune=4kc ");

    if (token_get("CONFIG_ADM5120_COMMON"))
	str_catprintf(&f, "-mgp32 -mlong32 -march=4kc -mtune=4kc ");

    if (token_get("CONFIG_ARMNOMMU"))
    {
	if (token_get("CONFIG_RG_OS_LINUX"))
	{
	    str_catprintf(&f, "-fomit-frame-pointer -fno-builtin ");
	    if (token_get("CONFIG_CPU_ARM7"))
		str_catprintf(&f, "-mcpu=arm7tdmi ");
	    if (token_get("CONFIG_CPU_ARM940T"))
		str_catprintf(&f, "-mcpu=arm9tdmi ");
	}
	else if (token_get("CONFIG_RG_OS_VXWORKS"))
	{
	    str_catprintf(&f, "-mno-thumb-interwork -mno-sched-prolog "
		"-fno-builtin ");
	    if (token_get("CONFIG_ARCH_IXP22X"))
		str_catprintf(&f, "-DCPU=ARM7TDMI ");
	}
    }
    
    if (token_get("CONFIG_SL2312_COMMON"))
	str_catprintf(&f, "-mapcs-32 -march=armv4 -mshort-load-bytes ");
    
    if (token_get("CONFIG_STRONGARM"))
    {
	if (token_get("CONFIG_RG_OLD_XSCALE_TOOLCHAIN"))
	{
	    str_catprintf(&f, "-mapcs-32 -march=armv4 -mtune=strongarm "
		"-mshort-load-bytes ");
	}
	else
	    str_catprintf(&f, "-mcpu=xscale -mtune=xscale -Wa,-mcpu=xscale ");
    }

    if (token_get("CONFIG_SOFT_FLOAT"))
	str_catprintf(&f, "-msoft-float ");

    if (token_get("CONFIG_TI_404_MIPS") && token_get("CONFIG_RG_OS_VXWORKS"))
    {
	str_catprintf(&f, "-mgp32 -mips2 -non_shared -DRW_MULTI_THREAD "
	    "-D_REENTRANT -DMIPSEB -msoft-float -DCPU=RC32364 -fno-common ");
    }
    
    if (token_get("CONFIG_ATHEROS_AR531X_MIPS") &&
	token_get("CONFIG_RG_OS_VXWORKS"))
    {
	str_catprintf(&f, "-mgp32 -mips32 -mips2 -G 0 -Wall "
	    "-Wno-char-subscripts -funsigned-char -DRW_MULTI_THREAD "
	    "-D_REENTRANT -fno-builtin -DMIPSEB -msoft-float -DPRJ_BUILD "
	    "-DAR5312 -DAR531X -DBUILD_AR5212 -DCPU=MIPS32 -DSOFT_FLOAT "
	    "-DBIG_ENDIAN=_BIG_ENDIAN -DVXWORKS -DBUILD_AP -DATHEROS "
	    "-ffunction-sections -fdata-sections ");
    }

    if (token_get("CONFIG_CX8620X"))
	str_catprintf(&f, "-mapcs-32 -march=armv5te -Wa,-march=armv5te ");

    if (token_get("CONFIG_PPC"))
	token_set_y("CONFIG_CC_HAVE_VA_COPY");
    
    if (token_get("CONFIG_I386_BOCHS") && token_get("CONFIG_RG_OS_VXWORKS"))
    {
	str_catprintf(&f, "-mpentium -DCPU=PENTIUM -DCPU_VARIANT=PENTIUM "
	    "-fno-builtin -fno-defer-pop ");
    }

    if (token_get("CONFIG_KS8695"))
    {
	str_catprintf(&f, "-Wall -Wstrict-prototypes -Wno-trigraphs -O2 "
	    "-fno-strict-aliasing -fno-common -fno-common -pipe -mapcs-32 "
	    "-march=armv4 -mtune=arm9tdmi -mshort-load-bytes");
    }	    
    
    if (token_get("CONFIG_RG_OS_VXWORKS"))
	str_catprintf(&f, "%s ", token_get_str("VX_INCLUDE_DIR"));
    
    if (token_get("CONFIG_INFINEON_AMAZON"))
	str_catprintf(&f, "-mabi=32 -march=4kc -Wa,-32 -Wa,-march=4kc ");

    token_set("ARCH_CFLAGS", "%s", f);
}

static void conf_arch_linux_cflags(void)
{
    if (token_get("CONFIG_BCM947XX"))
    {
	token_set("ARCH_LINUX_FLAGS", "%s  -Wa,--trap",
	    token_getz("ARCH_CFLAGS"));
    }
    
    if (token_get("CONFIG_BCM963XX"))
    {
	token_set("ARCH_LINUX_FLAGS", "%s  -Wa,--trap",
	    token_getz("ARCH_CFLAGS"));
    }
    
    if (token_get("CONFIG_INCAIP"))
    {
	token_set("ARCH_LINUX_FLAGS", "%s  -Wa,--trap",
	    token_getz("ARCH_CFLAGS"));
    }
    
    if (token_get("CONFIG_ADM5120_COMMON"))
    {
	token_set("ARCH_LINUX_FLAGS", "%s  -Wa,--trap",
	    token_getz("ARCH_CFLAGS"));
    }

    if (token_get("CONFIG_INFINEON_AMAZON"))
    {
	token_set("ARCH_LINUX_FLAGS", "%s  -Wa,--trap ",
	    token_getz("ARCH_CFLAGS"));
    }
}

static void conf_rg_linux_cflags(void)
{
    char *f = NULL;

    if (token_get("CONFIG_RG_OS_VXWORKS") && token_get("CONFIG_CNXT_ADSL"))
	str_catprintf(&f, "-mstructure-size-boundary=8 ");

    if (token_get("CONFIG_RG_OS_VXWORKS"))
	return;
    
    str_catprintf(&f, "-Wall -Wstrict-prototypes -Wno-trigraphs "
	"-fno-strict-aliasing -fno-common -pipe ");

    if (token_get("CONFIG_RG_DEV"))
	str_catprintf(&f, "-g ");
    
    if (!token_get("CONFIG_FRAME_POINTER"))
	str_catprintf(&f, "-fomit-frame-pointer ");

    if (token_get("CONFIG_SL2312_COMMON"))
    {
	str_catprintf(&f, "-Os -Uarm -mapcs-32 -D__LINUX_ARM_ARCH__=4 "
	    "-march=armv4 -Wa, -mshort-load-bytes -msoft-float %s ",
	    token_get_str("ARCH_CFLAGS"));
    }

    if (token_get("CONFIG_MPC8272ADS"))
    {
	str_catprintf(&f, "-Os -msoft-float %s -I%s ",
	    token_get_str("ARCH_CFLAGS"), token_get_str("LINUX_ARCH_PATH"));
    }

    if (token_get("CONFIG_IXP425_COMMON"))
    {
	str_catprintf(&f, "-mbig-endian -Os -Uarm -mapcs-32 "
	    "-D__LINUX_ARM_ARCH__=4 -mshort-load-bytes %s ",
	    token_get_str("ARCH_CFLAGS"));
	if (token_get("CONFIG_RG_OLD_XSCALE_TOOLCHAIN"))
	{
	    str_catprintf(&f, "-march=armv4 -Wa,-mxscale -mtune=strongarm "
		"-msoft-float ");
	}
    }

    if (token_get("CONFIG_CX8620X_COMMON"))
    {
	str_catprintf(&f, "-Os -Uarm -mapcs-32 -D__LINUX_ARM_ARCH__=5 "
	    "-march=armv5te %s ", token_getz("ARCH_CFLAGS"));
    }

    if (token_get("CONFIG_PCBOX") || token_get("CONFIG_RG_UML"))
    {
	str_catprintf(&f, "-O2 -mpreferred-stack-boundary=2 -march=i386 "
	    "-DUTS_MACHINE=\"i386\" %s ", token_getz("ARCH_CFLAGS"));
    }

    if (token_get("CONFIG_BCM947"))
    {
	str_catprintf(&f, "-O2 -mno-split-addresses -G 0 -mno-abicalls "
	    "-fno-pic -mlong-calls -DBCM47XX_CHOPS -DDMA %s ", 
	    token_getz("ARCH_CFLAGS"));
    }

    if (token_get("CONFIG_BCM963XX"))
    {
	str_catprintf(&f, "-O2 -mno-split-addresses -G 0 -mno-abicalls "
	    "-fno-pic -mlong-calls %s ", token_getz("ARCH_LINUX_FLAGS"));
    }

    if (token_get("CONFIG_INCAIP"))
    {
	str_catprintf(&f, "-O2 -G 0 -mno-abicalls -fno-pic -mlong-calls %s ",
	    token_getz("ARCH_LINUX_FLAGS"));
    }

    if (token_get("CONFIG_ADM5120_COMMON"))
    {
	str_catprintf(&f, "-O2 -G 0 -mno-abicalls -fno-pic -mlong-calls %s ",
	    token_getz("ARCH_LINUX_FLAGS"));
    }

    if (token_get("CONFIG_INFINEON_AMAZON"))
    {
	str_catprintf(&f, "-Os -G 0 -mno-abicalls -fno-pic -mlong-calls %s ",
	    token_getz("ARCH_LINUX_FLAGS"));
    }
    
    if (token_get("CONFIG_KS8695_COMMON"))
    {
	str_catprintf(&f, "-Wall -Wstrict-prototypes -Wno-trigraphs -Os "
	    "-fno-strict-aliasing -fno-common -Uarm -fno-common -pipe "
	    "-D__LINUX_ARM_ARCH__=5 -Wa, %s ",
	    token_getz("ARCH_LINUX_FLAGS"));
    }

    str_catprintf(&f, "-I%s/os/linux/include -D__KERNEL__ -D__TARGET__ ",
	BUILDDIR);

    token_set("RG_LINUX_CFLAGS", "%s", f);
}

static void conf_linux_cflags(void)
{
    token_set("LINUX_CFLAGS", "%s", token_getz("ENDIAN_CFLAGS"));
}

static void conf_host_ldflags(void)
{
    char *f = NULL;
    /* LOCAL_LDFLAGS: 
     * Make sure we are using the ld-so and linked agaist our libs
     */
    str_printf(&f, "-L%s/pkg/lib -Wl,--dynamic-linker,"
	"%s/i386-jungo-linux-gnu/lib/ld-2.3.2.so -Wl,-rpath,"
	"%s/i386-jungo-linux-gnu/lib ", RGSRC,
	token_get_str("I386_TOOLS_PREFIX"), 
	token_get_str("I386_TOOLS_PREFIX"));

    /* If using valgrind we dont want the --static flag */
    if (!token_get("CONFIG_RG_VALGRIND_LOCAL_TARGET"))
	str_catprintf(&f, "--static ");

    token_set("LOCAL_LDFLAGS", "%s", f);
    str_free(&f);
}

/* LDFLAGS are flags for GCC linkage, not flags for LD directly! */
static void conf_ldflags(void)
{
    char *f = NULL;

    str_catprintf(&f, "%s ", token_getz("ENDIAN_CFLAGS"));

    if (token_get("CONFIG_ARMNOMMU"))
    {
	if (token_get("CONFIG_RG_OS_LINUX"))
	    str_catprintf(&f, "-static Wl,-elf2flt ");
	else if (token_get("CONFIG_RG_OS_VXWORKS"))
	    str_catprintf(&f, "-X -N -Tldscript ");
    }

    if (token_get("CONFIG_RG_OS_VXWORKS"))
    {
	if (token_get("CONFIG_TI_404_MIPS") || token_get("CONFIG_I386_BOCHS")
	    || token_get("CONFIG_ATHEROS_AR531X_MIPS"))
	{
	    str_catprintf(&f, "-X -N ");
	}
    }

    if (token_get("HAVE_GC_SECTIONS") &&
	token_get("CONFIG_RG_FOOTPRINT_REDUCTION"))
    {
	str_catprintf(&f, "-Wl,--gc-sections ");
    }

    if (token_get("CONFIG_ULIBC") && !token_get("CONFIG_DYN_LINK"))
	str_catprintf(&f, "-static ");
    
    str_catprintf(&f, "-L%s/pkg/lib ", RGSRC);

    token_set("LDFLAGS_ENVIR", "%s", f);

    str_free(&f);
}

static void conf_host_cflags(void)
{
    char *f = NULL;

    if (token_get("CONFIG_RG_JPKG_SRC"))
	str_catprintf(&f, "-I%s/pkg ", RGSRC);

    str_catprintf(&f, "-I%s/pkg/include -D__HOST__ -O0 -g -Wall "
	"-Wstrict-prototypes --no-strict-aliasing", RGSRC);
    
    token_set("LOCAL_CFLAGS", "%s", f);

    str_free(&f);
}

static void conf_cflags(void)
{
    char *f = NULL;

    if (token_get("CONFIG_ARMNOMMU") && token_get("CONFIG_RG_OS_LINUX"))
	str_catprintf(&f, "-fpic -msingle-pic-base -D__PIC__ ");

    if (token_get("HAVE_GC_SECTIONS") &&
	token_get("CONFIG_RG_FOOTPRINT_REDUCTION"))
    {
	str_catprintf(&f, "-ffunction-sections -fdata-sections ");
    }

    if (!token_get("CONFIG_RG_OS_VXWORKS"))
	str_catprintf(&f, "-Wstrict-prototypes ");

    if (!token_get("CONFIG_I386_BOCHS"))
	str_catprintf(&f, "--no-strict-aliasing ");

    str_catprintf(&f, "%s %s -D__TARGET__ ", token_getz("RG_CFLAGS"),
	token_getz("ARCH_CFLAGS"));

    token_set("CFLAGS_ENVIR", "%s", f);
}

static void conf_openrg_lflags(void)
{
    char *suffix = token_get("CONFIG_DYN_LINK") ? "so" : "a";
    char *rg_gpl_ldlibs = NULL;
    char *librg = NULL, *liblocal_rg = NULL;

    token_set("OPENRG_LIB", "%s/pkg/util/libopenrg.%s", BUILDDIR, suffix);

    token_set("OPENRG_GPL_LIB", "%s/pkg/util/libopenrg_gpl.%s ", BUILDDIR,
	suffix);

    /* OPENRG_GPL_LDLIBS */
    str_catprintf(&rg_gpl_ldlibs, "-lopenrg_gpl ");
    if (token_get("CONFIG_GLIBC"))
	str_catprintf(&rg_gpl_ldlibs, "-lulibc_syslog ");
    token_set("OPENRG_GPL_LDLIBS", "%s ", rg_gpl_ldlibs);

    /* LIBS */
    str_catprintf(&librg, "-lopenrg ");
    str_catprintf(&liblocal_rg, "-llocal_openrg ");
    
    if (token_get("CONFIG_RG_OPENSSL"))
    {
	str_catprintf(&librg, "-lssl ");
	str_catprintf(&liblocal_rg, "-llocal_ssl ");
    }

    if (token_get("CONFIG_RG_CRYPTO"))
    {
	str_catprintf(&librg, "-lcrypto ");
	str_catprintf(&liblocal_rg, "-llocal_crypto ");
    }
    else
    {
	str_catprintf(&librg, "-lcrypto_rg ");
	str_catprintf(&liblocal_rg, "-llocal_crypto_rg ");
    }

    if (token_get("CONFIG_DYN_LINK"))
	str_catprintf(&librg, "-ldl ");

    str_catprintf(&librg, "-lopenrg -ljutil -lrg_config");
    str_catprintf(&liblocal_rg, "-llocal_openrg -llocal_jutil "
	"-llocal_rg_config");

    token_set("OPENRG_LDLIBS", "%s", librg);
    token_set("OPENRG_LOCAL_LDLIBS", "%s", liblocal_rg);

    token_set("MGT_LDLIBS", "-lmgt ");
    token_set("MGT_LOCAL_LDLIBS", "-llocal_mgt ");
    token_set("LANG_LDLIBS", "-llang ");
    token_set("LANG_LOCAL_LDLIBS", "-llocal_lang ");
}

void conf_doctools(void)
{
    char *latex_exe = sys_get(NULL,
	"latex --version 2>/dev/null | head -n 1 | cut -f 1 -d\" \"");

    printf("Detecting latex executable: ");
    if (!latex_exe || !*latex_exe)
    {
	printf("Not found.\n");
	return;
    }
    str_chomp(&latex_exe);
    if (!strcmp(latex_exe, "e-TeX"))
    {
	printf("found e-Tex\n");
	token_set("TEXMFCNF", "%s/pkg/doc/texmf", RGSRC);
    }
    else if (!strcmp(latex_exe, "TeX"))
    {
	printf("found Tex\n");
	token_set("TEXMF",
	    "{/usr/local/openrg/share/texmf,!!/usr/share/texmf}");
    }
    else
	printf("Unknown version (%s)\n", latex_exe);

    str_free(&latex_exe);
}

static void conf_arch(void)
{
    char *arch = token_getz("ARCH");
    char *jpkg_src = token_get_str("CONFIG_RG_JPKG_SRC");
    
    if (!strcmp(arch, "arm") || jpkg_src)
	token_set_y("CONFIG_ARM");
    if (!strcmp(arch, "armnommu") || jpkg_src)
	token_set_y("CONFIG_ARMNOMMU");
    if (!strcmp(arch, "i386") || jpkg_src)
	token_set_y("CONFIG_I386");
    if (!strcmp(arch, "mips") || jpkg_src)
	token_set_y("CONFIG_MIPS");
    if (!strcmp(arch, "ppc") || jpkg_src)
	token_set_y("CONFIG_PPC");
    if (!strcmp(arch, "um") || jpkg_src)
	token_set_y("CONFIG_UM");
}

void config_host(void)
{
    BUILDDIR = getenv("BUILDDIR");
    RGSRC = getenv("RGSRC");
    conf_ostype();
    conf_os_path();
    conf_ccache();
    conf_toolchain();
    conf_host_tools();
    conf_cc_for_build();
    conf_host_cflags();
    conf_host_ldflags();
    conf_host_general();
    conf_arch();
    if (token_get("CONFIG_RG_JPKG_SRC"))
	return;
    conf_endian_flags();
    conf_target_machine();
    conf_tools_prefix();
    conf_ulibc_in_tooldchain();
    conf_glibc_in_tooldchain();
    conf_cc();
    conf_target_cc();
    conf_gcc_version();
    conf_tools();
    conf_openrg_img();
    conf_prod_kernel_image();
    conf_linux_arch_path();
    conf_compressed_disk();
    conf_os_target();
    conf_target_general();
    conf_trx();
    conf_rg_cpu();
    conf_flash_img_offset();
    conf_have_gc_sections();
    conf_gcc_has_size_optimize();
    conf_rg_cflags();
    conf_arch_cflags();
    conf_arch_linux_cflags();
    conf_rg_linux_cflags();
    conf_linux_cflags();
    conf_ldflags();
    conf_cflags();
    conf_openrg_lflags();
    conf_doctools();
}
