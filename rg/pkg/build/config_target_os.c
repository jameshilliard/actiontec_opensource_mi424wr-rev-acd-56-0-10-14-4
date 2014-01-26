/****************************************************************************
 *
 * rg/pkg/build/config_target_os.c
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

#include "config_opt.h"
#include "create_config.h"

void target_os_features(char *os)
{
    /* target OS */
    token_set("CONFIG_RG_OS", os);

    if (os && !strcmp(os, "LINUX_24"))
	token_set_y("CONFIG_RG_OS_LINUX_24");
    else if (os && !strcmp(os, "ECOS"))
	token_set_y("CONFIG_RG_OS_ECOS");
    else if (os && !strcmp(os, "VXWORKS"))
	token_set_y("CONFIG_RG_OS_VXWORKS");

    /* VxWorks special behaviour */
    if (token_get("CONFIG_RG_OS_VXWORKS"))
    {
	token_set_y("CONFIG_RG_NOT_UNIX");
	token_set_y("CONFIG_RG_EFSS");
        if (token_get("CONFIG_RG_DEV"))
	    token_set_y("CONFIG_VX_SYMTBL");
	token_set_y("CONFIG_RG_VX_TIMERS_TASK");
    }

    /* Linux Kernel generic configs */
    if (token_get("CONFIG_RG_OS_LINUX_24"))
    {
	token_set_y("CONFIG_UID16");
	token_set_y("CONFIG_RWSEM_GENERIC_SPINLOCK");
	token_set_y("CONFIG_RG_TTYP");
	token_set_y("CONFIG_IP_MULTICAST");
	token_set_y("CONFIG_IP_ADVANCED_ROUTER");
	token_set_y("CONFIG_UNIX");
	token_set_y("CONFIG_RG_OS_LINUX");
	token_set_y("CONFIG_INET");
	token_set_y("CONFIG_MODULES");
	token_set_y("CONFIG_NET");
	token_set_y("CONFIG_NETDEVICES");
	token_set_y("CONFIG_NETLINK_DEV");
	token_set_y("CONFIG_FILTER");
	token_set_y("CONFIG_NET_ETHERNET");
	token_set_y("CONFIG_PACKET");
	token_set_y("CONFIG_SYSVIPC");

	if (!token_get("CONFIG_RG_RGLOADER"))
	{
	    token_set_y("CONFIG_PROC_FS");
	    token_set_m("CONFIG_RG_IPV4");
	}
	
	/* Override only if LZMA is not fast enough */
	token_set("CONFIG_RG_KERNEL_COMP_METHOD", "lzma");
    }

    /* Set CONFIG_RG_TARGET_xxx */
    if (token_get("CONFIG_RG_OS_LINUX"))
	token_set_y("CONFIG_RG_TARGET_LINUX");
    if (token_get("CONFIG_RG_OS_ECOS"))
	token_set_y("CONFIG_RG_TARGET_ECOS");
    if (token_get("CONFIG_RG_OS_VXWORKS"))
	token_set_y("CONFIG_RG_TARGET_VXWORKS");
}
