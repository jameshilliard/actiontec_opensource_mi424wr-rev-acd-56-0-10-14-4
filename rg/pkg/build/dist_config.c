/****************************************************************************
 *
 * rg/pkg/build/dist_config.c
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

#include <string.h>
#include "config_opt.h"
#include "create_config.h"
#include <sys/stat.h>
#include <unistd.h>

static void set_jpkg_basic_modules(void)
{
    token_set_y("CONFIG_RG_SMB");

    token_set_y("CONFIG_PROC_FS");
    token_set_m("CONFIG_RG_IPV4");

    /* Currently I386 feature */
    enable_module("MODULE_RG_FOUNDATION");
    enable_module("MODULE_RG_ZERO_CONFIGURATION");
    enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
    enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
    enable_module("MODULE_RG_IPSEC");
    enable_module("MODULE_RG_VLAN");
}

static void set_jpkg_modules(void)
{
    set_jpkg_basic_modules();
    enable_module("MODULE_RG_QOS");
    enable_module("MODULE_RG_IPV6");
    enable_module("MODULE_RG_PPP");
    enable_module("MODULE_RG_PPTP");
    enable_module("MODULE_RG_L2TP_CLIENT");
    enable_module("MODULE_RG_ADVANCED_ROUTING");
    enable_module("MODULE_RG_DSLHOME");
    enable_module("MODULE_RG_SNMP");
    enable_module("MODULE_RG_URL_FILTERING");
    enable_module("MODULE_RG_MAIL_FILTER");
}

static int stat_lic_file(char *path)
{
    struct stat s;
    int ret = stat(path, &s);

    printf("Searching for license file in %s: %sfound\n", path,
	ret ? "not " : "");
    return ret;
}

char *set_dist_license(void)
{
#define DEFAULT_LIC_DIR "pkg/license/licenses/"
#define DEFAULT_LIC_FILE "license.lic"
    char *lic = NULL;

    if (IS_DIST("RTA770W"))
	lic = DEFAULT_LIC_DIR "belkin.lic";
    else if (IS_DIST("MI424WR") || IS_DIST("MI424WR_VOIP") ||
	IS_DIST("RI408") || IS_DIST("UML_BHR") || IS_DIST("VI414WG") ||
	IS_DIST("VI414WG_ETH") || IS_DIST("KI414WG") || 
	IS_DIST("KI414WG_ETH") || IS_DIST("BA214WG") || IS_DIST("MI524WR"))
    {
	/* TODO: Add BA214WG when correct license is generated */
	lic = DEFAULT_LIC_DIR "actiontec.lic";
    }
    else if (!stat_lic_file(DEFAULT_LIC_DIR DEFAULT_LIC_FILE))
	lic = DEFAULT_LIC_DIR DEFAULT_LIC_FILE;
    else if (!stat_lic_file(DEFAULT_LIC_FILE))
	lic = DEFAULT_LIC_FILE;

    if (lic)
	token_set("LIC", lic);
    return lic;
}

void distribution_features()
{
    if (!dist)
	conf_err("ERROR: DIST is not defined\n");

    /* MIPS */
    if (IS_DIST("ADM5120_LSP"))
    {
	hw = "ADM5120P";
	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_FRAME_POINTER");
    }
    else if (IS_DIST("RGLOADER_ADM5120"))
    {
	hw = "ADM5120P";
	os = "LINUX_24";
	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_FRAME_POINTER");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("ADM5120_ATA"))
    {
	hw = "ADM5120P";
	os = "LINUX_24";
	
	/* OpenRG Feature set */
	token_set_y("CONFIG_RG_FOUNDATION_CORE");
	token_set_y("CONFIG_RG_CHECK_BAD_REBOOTS");
	/* From MODULE_RG_ZERO_CONFIGURATION take only this */
	token_set_y("CONFIG_AUTO_LEARN_DNS");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_VOIP_RV_SIP");
    	token_set_y("CONFIG_RG_EVENT_LOGGING");
	token_set_y("CONFIG_RG_STATIC_ROUTE"); /* Static Routing */
	token_set_y("CONFIG_RG_UCD_SNMP"); /* SNMP v1/2 only */

	/* OpenRG HW support */
	enable_module("CONFIG_HW_DSP");
	token_set_y("CONFIG_HW_ETH_WAN");

	token_set_y("CONFIG_RG_PROD_IMG");
	token_set_y("CONFIG_DEF_WAN_ALIAS_IP");
    }
    else if (IS_DIST("ADM5120_VGW") || IS_DIST("ADM5120_VGW_OSIP"))
    {
	hw = "ADM5120P";
	os = "LINUX_24";
	
	/* OpenRG Feature set */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
        enable_module("MODULE_RG_VLAN");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_DSP");

	token_set_y("CONFIG_RG_PROD_IMG");

	/* VoIP */
	if (IS_DIST("ADM5120_VGW_OSIP"))
	    enable_module("MODULE_RG_VOIP_OSIP");
	else
	{
	    enable_module("MODULE_RG_VOIP_RV_SIP");
	    enable_module("MODULE_RG_VOIP_RV_H323");
	    enable_module("MODULE_RG_VOIP_RV_MGCP");
	}
    }
    else if (IS_DIST("INCAIP_LSP"))
    {
	hw = "INCAIP_LSP";
	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_FRAME_POINTER");
	token_set_y("CONFIG_VINETIC_TAPIDEMO");
    }
    else if (IS_DIST("RGLOADER_INCAIP"))
    {
	hw = "INCAIP";
	token_set_y("CONFIG_RG_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("INCAIP_IPPHONE"))
    {
	hw = "INCAIP";

	/* OpenRG Feature set */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	token_set_y("CONFIG_RG_STATIC_ROUTE"); /* Static Routing */
    	token_set_y("CONFIG_RG_EVENT_LOGGING"); /* Event Logging */

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	enable_module("CONFIG_HW_DSP");
	token_set_y("CONFIG_HW_KEYPAD");
	token_set_y("CONFIG_HW_LEDS");

	token_set_y("CONFIG_DYN_LINK");
	token_set_y("CONFIG_DEF_WAN_ALIAS_IP");
	token_set_y("CONFIG_RG_PROD_IMG");
    }
    else if (IS_DIST("RGLOADER_FLEXTRONICS"))
    {
	hw = "FLEXTRONICS";
	token_set_y("CONFIG_RG_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("FLEXTRONICS_IPPHONE"))
    {
	hw = "FLEXTRONICS";

	/* OpenRG Feature set */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	token_set_y("CONFIG_RG_STATIC_ROUTE"); /* Static Routing */
    	token_set_y("CONFIG_RG_EVENT_LOGGING"); /* Event Logging */

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	enable_module("CONFIG_HW_DSP");
	token_set_y("CONFIG_HW_KEYPAD");
	token_set_y("CONFIG_HW_LEDS");

	token_set_y("CONFIG_DYN_LINK");
	token_set_y("CONFIG_DEF_WAN_ALIAS_IP");
    }
    else if (IS_DIST("INCAIP_ATA") || IS_DIST("INCAIP_ATA_OSIP"))
    {
	hw = "ALLTEK";

	/* OpenRG Feature set */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
    	token_set_y("CONFIG_RG_EVENT_LOGGING");
	token_set_y("CONFIG_RG_STATIC_ROUTE"); /* Static Routing */

	/* OpenRG HW support */
	enable_module("CONFIG_HW_DSP");
	token_set_y("CONFIG_HW_ETH_WAN");

	token_set_y("CONFIG_DYN_LINK");
	token_set_y("CONFIG_DEF_WAN_ALIAS_IP");
	token_set_y("CONFIG_RG_PROD_IMG");

	/* VoIP */
	if (IS_DIST("INCAIP_ATA_OSIP"))
	    enable_module("MODULE_RG_VOIP_OSIP");
	else
	{
	    enable_module("MODULE_RG_VOIP_RV_SIP");
	    enable_module("MODULE_RG_VOIP_RV_MGCP");
	    enable_module("MODULE_RG_VOIP_RV_H323");
	}
    }
    else if (IS_DIST("RGLOADER_ALLTEK") ||
	IS_DIST("RGLOADER_ALLTEK_FULLSOURCE"))
    {
	hw = "ALLTEK";

	token_set_y("CONFIG_RG_RGLOADER");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_LAN");

    }
    else if (IS_DIST("INCAIP_VGW") || IS_DIST("INCAIP_VGW_OSIP"))
    {
	hw = "ALLTEK_VLAN";

    	token_set_y("CONFIG_RG_SMB");

	/* OpenRG Feature set */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_PPP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_L2TP_CLIENT");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
        enable_module("MODULE_RG_IPV6");
        enable_module("MODULE_RG_URL_FILTERING");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_RG_PROD_IMG");
	token_set_y("CONFIG_DYN_LINK");
	token_set_y("CONFIG_RG_ATA");

	/* VoIP */
	if (IS_DIST("INCAIP_VGW_OSIP"))
	    enable_module("MODULE_RG_VOIP_OSIP");
	else
	{
	    enable_module("MODULE_RG_VOIP_RV_SIP");
	    enable_module("MODULE_RG_VOIP_RV_H323");
	    enable_module("MODULE_RG_VOIP_RV_MGCP");
	}
    }
    else if (IS_DIST("INCAIP_FULLSOURCE"))
    {
	hw = "ALLTEK_VLAN";

    	token_set_y("CONFIG_RG_SMB");

	/* OpenRG Feature set */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_PPP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_L2TP_CLIENT");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
        enable_module("MODULE_RG_IPV6");
        enable_module("MODULE_RG_URL_FILTERING");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_RG_PROD_IMG");
	token_set_y("CONFIG_DYN_LINK");
    }
    else if (IS_DIST("BCM94702"))
    {
	hw = "BCM94702";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_RG_PROD_IMG");
    }
    else if (IS_DIST("BCM94704"))
    {
	hw = "BCM94704";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_SNMP");
    	token_set_y("CONFIG_RG_EVENT_LOGGING"); /* Event Logging */
	token_set_y("CONFIG_RG_ENTFY");	/* Email notification */

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_BCM43XX");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0", "eth0", NULL);

	token_set_y("CONFIG_RG_PROD_IMG");
    }
    else if (IS_DIST("USI_BCM94712"))
    {
	hw = "BCM94712";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
        enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP_CLIENT");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_BCM43XX");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0.0", "eth0", NULL);

	token_set_y("CONFIG_DYN_LINK");
	token_set_y("CONFIG_RG_PROD_IMG");
    }
    else if (IS_DIST("SRI_USI_BCM94712"))
    {
	hw = "BCM94712";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");

	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_URL_FILTERING");
	token_set("CONFIG_RG_SURFCONTROL_PARTNER_ID", "6003");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_SNMP");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_80211G_BCM43XX");
	
	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0.0", "eth0", NULL);

	/* The Broadcom nas application is dynamically linked */
	token_set_y("CONFIG_DYN_LINK");

	/* Wireless GUI options */
	/* Do NOT show Radius icon in advanced */
	token_set_y("CONFIG_RG_RADIUS_IN_CONN");

	/* Dist specific configuration */
	token_set_y("CONFIG_RG_PROD_IMG");
    }
    else if (IS_DIST("RTA770W"))
    {
	hw = "RTA770W";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	/* broadcom nas (wpa application) needs ulibc.so so we need to compile
	 * openrg dynamically */
	token_set_y("CONFIG_DYN_LINK");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	token_set_y("CONFIG_RG_IGD_XBOX");
	enable_module("MODULE_RG_DSL");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_BCM43XX");
	enable_module("CONFIG_HW_USB_RNDIS");
	token_set_m("CONFIG_HW_BUTTONS");
        token_set_y("CONFIG_HW_LEDS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0", "usb0", "wl0", NULL);

	token_set_y("CONFIG_GUI_BELKIN");
	token_set("RG_PROD_STR", "Prodigy infinitum");
	token_set_y("CONFIG_RG_DSL_CH");
	token_set_y("CONFIG_RG_PPP_ON_DEMAND_AS_DEFAULT");
	/* Belkin's requirement - 3 hours of idle time */
	token_set("CONFIG_RG_PPP_ON_DEMAND_DEFAULT_MAX_IDLE_TIME", "10800");
	/* from include/enums.h PPP_COMP_ALLOW is 1 */
	token_set("CONFIG_RG_PPP_DEFAULT_BSD_COMPRESSION", "1");
	/* from include/enums.h PPP_COMP_ALLOW is 1 */
	token_set("CONFIG_RG_PPP_DEFAULT_DEFLATE_COMPRESSION", "1");
	/* Download image to memory before flashing
	 * Only one image section in flash, enough memory */
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");
	/* For autotest and development purposes, Spanish is the default
	 * language allowing an override */
	if (!token_get_str("CONFIG_RG_DIST_LANG"))
	    token_set("CONFIG_RG_DIST_LANG", "spanish_belkin");
	token_set_y("CONFIG_RG_CFG_SERVER");
	token_set_y("CONFIG_RG_STATIC_ROUTE");
	token_set_y("CONFIG_RG_OSS_RMT");
	token_set_y("CONFIG_RG_RMT_MNG");
	token_set_y("CONFIG_RG_NON_ROUTABLE_LAN_DEVICE_IP");
    }
    else if (IS_DIST("RTA770W_EVAL"))
    {
	hw = "RTA770W";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_DSL");
	token_set_y("CONFIG_RG_STATIC_ROUTE");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_BCM43XX");
	enable_module("CONFIG_HW_USB_RNDIS");
	token_set_m("CONFIG_HW_BUTTONS");
        token_set_y("CONFIG_HW_LEDS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0", "usb0", "wl0", NULL);

	/* Download image to memory before flashing
	 * Only one image section in flash, enough memory */
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");
	/* broadcom nas (wpa application) needs ulibc.so so we need to compile
	 * openrg dynamically */
	token_set_y("CONFIG_DYN_LINK");
    }
    else if (IS_DIST("RGLOADER_RTA770W"))
    {
	hw = "RTA770W";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_BCM963XX_BOOTSTRAP");
	token_set_m("CONFIG_RG_KRGLDR");

	/* Create flash image since it rgloader does not reside in section 0.
	 * This flash image (openrg.prod) is burned to address 0, instead of 
	 * rgloader.img.
	 */
	token_set_y("CONFIG_RG_PROD_IMG");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("GTWX5803") || IS_DIST("GTWX5803_EVAL"))
    {
	hw = "GTWX5803";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	token_set_y("CONFIG_RG_WPA");
	token_set_y("CONFIG_RG_WEP");
	/* broadcom nas (wpa application) needs ulibc as shared object */
	token_set_y("CONFIG_ULIBC_SHARED");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_DSL");
	enable_module("MODULE_RG_URL_FILTERING");

	if (IS_DIST("GTWX5803"))
	{
	    enable_module("MODULE_RG_VLAN");
	    enable_module("MODULE_RG_PPTP");
	    enable_module("MODULE_RG_L2TP_CLIENT");
	    enable_module("MODULE_RG_MAIL_FILTER");
	    token_set_y("CONFIG_ZERO_CONFIGURATION");
	    enable_module("MODULE_RG_IPSEC");
	    enable_module("MODULE_RG_IPV6");
	    enable_module("MODULE_RG_DSLHOME");
	    enable_module("MODULE_RG_QOS");
	}

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_BCM43XX");
	enable_module("CONFIG_HW_USB_RNDIS");
	token_set_m("CONFIG_HW_BUTTONS");
        token_set_y("CONFIG_HW_LEDS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0", "usb0", "wl0", NULL);
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_MIPSEB");
    }
    else if (IS_DIST("RGLOADER_GTWX5803"))
    {
	hw = "GTWX5803";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_BCM963XX_BOOTSTRAP");
	token_set_m("CONFIG_RG_KRGLDR");

	/* Create flash image since it rgloader does not reside in section 0.
	 * This flash image (openrg.prod) is burned to address 0, instead of 
	 * rgloader.img.
	 */
	token_set_y("CONFIG_RG_PROD_IMG");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("WRT54G"))
    {
	hw = "WRT54G";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
        enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_SNMP");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_BCM43XX");
	token_set_m("CONFIG_HW_BUTTONS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0.2", "eth0", NULL);

	token_set_y("CONFIG_ARCH_BCM947_CYBERTAN");
	token_set_y("CONFIG_RG_BCM947_NVRAM_CONVERT");
	token_set_y("CONFIG_DYN_LINK");
	token_set_y("CONFIG_GUI_LINKSYS");
	token_set_y("CONFIG_RG_PROD_IMG");
    }
    else if (IS_DIST("CX82100_SCHMID"))
    {
	hw = "CX82100";
	os = "LINUX_22";
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_USB_RNDIS");

	token_set_y("CONFIG_RG_PROD_IMG");
	token_set_y("CONFIG_RG_TODC");
    }
    else if (IS_DIST("X86_FRG_TMT")) /* x86 */
    {
	hw = "PCBOX_EEP_EEP";
	
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_VLAN");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_RG_PROD_IMG");
    }
    else if (IS_DIST("RGLOADER_X86_TMT"))
    {
	hw = "PCBOX_EEP_EEP";

	token_set_y("CONFIG_RG_RGLOADER");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_RG_PROD_IMG");
    }
    else if (IS_DIST("ALLWELL_RTL_RTL"))
    {
	hw = "ALLWELL_RTL_RTL";

	token_set_y("PACKAGE_OPENRG_VPN_GATEWAY");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_RG_PROD_IMG");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_I386");
    }
    else if (IS_DIST("ALLWELL_RTL_RTL_SSI"))
    {
	hw = "ALLWELL_RTL_RTL";
	token_set_y("PACKAGE_OPENRG_VPN_GATEWAY");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_RG_PROD_IMG");
	token_set_y("CONFIG_RG_SSI_PAGES");
    }
    else if (IS_DIST("ALLWELL_RTL_EEP"))
    {
	hw = "ALLWELL_RTL_EEP";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

    }
    else if (IS_DIST("ALLWELL_RTL_RTL_ALADDIN"))
    {
	hw = "ALLWELL_RTL_RTL";

	token_set_y("PACKAGE_OPENRG_VPN_GATEWAY");
	enable_module("MODULE_RG_MAIL_FILTER");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_RG_PROD_IMG");
    }
    else if (IS_DIST("ALLWELL_RTL_RTL_WELLTECH"))
    {
	hw = "ALLWELL_RTL_RTL";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_GUI_WELLTECH");
	token_set_y("CONFIG_RG_PROD_IMG");
    }
    /* XScale IXP425 based boards */
    else if (IS_DIST("COYOTE_WIRELESS_80211B")) 
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211B_PRISM2");
        enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);

	token_set_y("CONFIG_IXP425_COMMON_RG");
    }
    else if (IS_DIST("COYOTE_UMEC"))
    {
	hw = "COYOTE_WIRELESS";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	enable_module("MODULE_RG_ADVANCED_ROUTING");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_80211G_RALINK_RT2560");
	token_set_y("CONFIG_HW_BUTTONS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "ra0", NULL);
	
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5B");
    }
    else if (IS_DIST("COYOTE_WIRELESS_ISL") ||
	IS_DIST("COYOTE_WIRELESS_SOFTMAC") ||
	IS_DIST("COYOTE_WIRELESS_RALINK"))
    {
	char *wireless_dev = NULL;

	hw = "COYOTE";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP_CLIENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	token_set_y("CONFIG_RG_PRINT_SERVER");
	enable_module("CONFIG_RG_LPD");
	token_set_y("CONFIG_RG_IPP");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
	if (IS_DIST("COYOTE_WIRELESS_ISL"))
	{
	    enable_module("CONFIG_HW_80211G_ISL38XX");
	    wireless_dev = "eth0";
	}
	if (IS_DIST("COYOTE_WIRELESS_SOFTMAC"))
	{
	    enable_module("CONFIG_HW_80211G_ISL_SOFTMAC");
	    wireless_dev = "eth0";
	}
	if (IS_DIST("COYOTE_WIRELESS_RALINK"))
	{
	    enable_module("CONFIG_HW_80211G_RALINK_RT2560");
	    wireless_dev = "ra0";
	}
        enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", wireless_dev,
	    NULL);

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_RG_PROD_IMG");
    }
    else if (IS_DIST("COYOTE_BHR"))
    {
	hw = "COYOTE";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	token_set_y("CONFIG_RG_8021Q_IF");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");

	token_set_m("CONFIG_RG_PPPOE_RELAY"); /* PPPoE Relay */
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_QOS");

	token_set_y("CONFIG_GUI_ACTIONTEC");
	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_RALINK_RT2560");

	token_set("CONFIG_RG_SSID_NAME", "openrg");
	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "ra0", NULL);
	
	/* Dist specific configuration */
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_RG_PROD_IMG");

	/* Tasklet me harder */
	token_set_y("CONFIG_TASKLET_ME_HARDER");

	/* Make readonly GUI */
	token_set_y("CONFIG_RG_WBM_READONLY_USERS_GROUPS");
    }

    else if (IS_DIST("RGLOADER_USR8200"))
    {
	hw = "USR8200";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("USR8200_EVAL"))
    {
	hw = "USR8200";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP_CLIENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
	token_set_y("CONFIG_HW_USB_HOST_UHCI");
	token_set_y("CONFIG_HW_FIREWIRE");
	token_set_y("CONFIG_HW_FIREWIRE_STORAGE");
	token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS1");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set_y("CONFIG_RG_DATE");
    }
    else if (IS_DIST("USR8200") || IS_DIST("USR8200_ALADDIN"))
    {
	hw = "USR8200";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	token_set_y("CONFIG_RG_SURFCONTROL");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
	token_set_y("CONFIG_HW_USB_HOST_UHCI");
	token_set_y("CONFIG_HW_FIREWIRE");
	token_set_y("CONFIG_HW_FIREWIRE_STORAGE");
	token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("CONFIG_HW_ENCRYPTION");
        token_set_y("CONFIG_HW_LEDS");
        token_set_y("CONFIG_HW_CLOCK");

	token_set_y("CONFIG_RG_FW_ICSA");
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set_y("CONFIG_RG_PROD_IMG");
	token_set_y("CONFIG_RG_DATE");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");
	token_set_y("CONFIG_RG_AUTOTEST");
	token_set_y("CONFIG_STOP_ON_INIT_FAIL");
	token_set("CONFIG_RG_SURFCONTROL_PARTNER_ID", "6002");
	token_set_y("CONFIG_RG_DHCPR");

	enable_module("MODULE_RG_QOS");
	
	/* All WBM pages expire with session */
	token_set_y("CONFIG_WBM_PAGES_EXPIRE");

	/* Include automatic daylight saving time calculation */
	token_set_y("CONFIG_RG_TZ_FULL");
	token_set("CONFIG_RG_TZ_YEARS", "5");

	if (IS_DIST("USR8200_ALADDIN"))
	    enable_module("MODULE_RG_MAIL_FILTER");
    }
    else if (IS_DIST("USR8200_TUTORIAL"))
    {
	hw = "USR8200";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP_CLIENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
	token_set_y("CONFIG_HW_USB_HOST_UHCI");
	token_set_y("CONFIG_HW_FIREWIRE");
	token_set_y("CONFIG_HW_FIREWIRE_STORAGE");
	token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("CONFIG_HW_ENCRYPTION");
        token_set_y("CONFIG_HW_LEDS");
        token_set_y("CONFIG_HW_CLOCK");

	token_set_y("CONFIG_RG_DATE");
	token_set_y("CONFIG_RG_TUTORIAL");
	token_set_y("CONFIG_STOP_ON_INIT_FAIL");
	token_set_y("CONFIG_IXP425_COMMON_RG");

	/* All WBM pages expire with session */
	token_set_y("CONFIG_WBM_PAGES_EXPIRE");

	/* Include automatic daylight saving time calculation */
	token_set_y("CONFIG_RG_TZ_FULL");
	token_set("CONFIG_RG_TZ_YEARS", "5");
    }
    else if (IS_DIST("BAMBOO") || IS_DIST("BAMBOO_ALADDIN"))
    {
	hw = "BAMBOO"; 

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_FILESERVER");
	token_set_y("CONFIG_RG_PROXY_ARP");
	token_set_y("CONFIG_RG_ENTFY");	/* Email notification */
    	token_set_y("CONFIG_RG_EVENT_LOGGING"); /* Event Logging */
	token_set_y("CONFIG_RG_UCD_SNMP"); /* SNMP v1/v2 */
	token_set_y("CONFIG_RG_8021X_TLS");
	token_set_y("CONFIG_RG_8021X_RADIUS");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211B_PRISM2");
        enable_module("CONFIG_HW_DSP");
	token_set_m("CONFIG_HW_USB_HOST_EHCI");
	token_set_m("CONFIG_HW_USB_HOST_UHCI");
	token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("CONFIG_HW_ENCRYPTION");
        token_set_y("CONFIG_HW_CAMERA_USB_OV511");
        token_set_m("CONFIG_HW_PCMCIA_CARDBUS");
	token_set_m("CONFIG_HW_BUTTONS");
	token_set_m("CONFIG_HW_LEDS");

	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_RG_PROD_IMG");
	token_set_y("CONFIG_IXP425_COMMON_RG");

	/* PPTP, PPPoE */
        token_set("CONFIG_RG_PPTP_ECHO_INTERVAL", "20");
        token_set("CONFIG_RG_PPTP_ECHO_FAILURE", "3");
        token_set("CONFIG_PPPOE_MAX_RETRANSMIT_TIMEOUT", "64");

	/* DSR support */
	token_set_y("CONFIG_IXP425_DSR");
	token_set_y("CONFIG_RG_OLD_XSCALE_TOOLCHAIN");

	if (IS_DIST("BAMBOO_ALADDIN"))
	{
	    enable_module("MODULE_RG_MAIL_FILTER");
	    enable_module("MODULE_RG_PRINTSERVER");
	    token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");
	}
    }
    else if (IS_DIST("IXDP425_CAVIUM"))
    {
	hw = "IXDP425";
	
	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP_CLIENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");	
    }
    else if (IS_DIST("RGLOADER_CENTROID"))
    {
	hw = "CENTROID";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_SL2312_COMMON");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
    }
    else if (IS_DIST("CENTROID"))
    {
	hw = "CENTROID";
	
	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_VLAN");

	enable_module("MODULE_RG_IPSEC");
	token_set_y("CONFIG_RG_EVENT_LOGGING");
	
	/* XXX this is a workaround until B22301 is resolved */
	token_set_y("CONFIG_RG_THREADS");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
 	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
	token_set_y("CONFIG_HW_USB_STORAGE");
	
	token_set_y("CONFIG_SL2312_COMMON_RG");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "8"); 

	dev_add_bridge("br0", DEV_IF_NET_INT, "sl0", "ra0", NULL);
    }
    else if (IS_DIST("IXDP425_GST"))
    {
	hw = "IXDP425";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_VLAN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211G_ISL38XX");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_y("CONFIG_HW_BUTTONS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");
	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
    }
    else if (IS_DIST("MATECUMBE"))
    {
	hw = dist;

	token_set_y("CONFIG_RG_SMB");
	token_set_y("CONFIG_ARM_24_FAST_MODULES");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP_CLIENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);
    }
    else if (IS_DIST("IXDP425") || IS_DIST("IXDP425_FULLSOURCE"))
    {
	hw = "IXDP425";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP_CLIENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20150");
	token_set_y("CONFIG_IXP425_ADSL_USE_MPHY");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);
    }
    else if (IS_DIST("IXDP425_WIRELESS"))
    {
	hw = "IXDP425";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP_CLIENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211G_ISL38XX");
        enable_module("CONFIG_HW_ENCRYPTION");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20150");
	token_set_y("CONFIG_IXP425_ADSL_USE_MPHY");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
    }
    else if (IS_DIST("RGLOADER_KINGSCANYON"))
    {
	hw = "KINGSCANYON";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("RGLOADER_KINGSCANYON_LSP"))
    {
	hw = "KINGSCANYON";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_LSP_FLASH_LAYOUT");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("KINGSCANYON_WIRELESS") || IS_DIST("KINGSCANYON"))
    {
	hw = "KINGSCANYON";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP_CLIENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	token_set_y("CONFIG_RG_PRINT_SERVER");
	enable_module("CONFIG_RG_LPD");
	token_set_y("CONFIG_RG_IPP");
	if (IS_DIST("KINGSCANYON_WIRELESS"))
	    enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");
	if (IS_DIST("KINGSCANYON_WIRELESS"))
	    enable_module("CONFIG_HW_80211G_ISL38XX");

	if (IS_DIST("KINGSCANYON_WIRELESS"))
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);
	else
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);
    }
    else if (IS_DIST("KINGSCANYON_LSP"))
    {
	hw = "KINGSCANYON";

	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_SIMPLE_RAMDISK");
	token_set_y("CONFIG_LSP_FLASH_LAYOUT");
    }
    else if (IS_DIST("RGLOADER_ROCKAWAYBEACH"))
    {
	hw = "ROCKAWAYBEACH";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("ROCKAWAYBEACH") || IS_DIST("ROCKAWAYBEACH_WIRELESS"))
    {
	hw = "ROCKAWAYBEACH";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_L2TP_CLIENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	token_set_y("CONFIG_RG_PRINT_SERVER");
	enable_module("CONFIG_RG_LPD");
	token_set_y("CONFIG_RG_IPP");
	if (IS_DIST("ROCKAWAYBEACH_WIRELESS"))
	    enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");
	if (IS_DIST("ROCKAWAYBEACH_WIRELESS"))
	    enable_module("CONFIG_HW_80211G_ISL38XX");

	if (IS_DIST("ROCKAWAYBEACH_WIRELESS"))
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);
	else
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);
    }
    else if (IS_DIST("IXDP425_NETKLASS"))
    {
	hw = "IXDP425";

	enable_module("MODULE_RG_FOUNDATION");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	token_set_y("CONFIG_RG_ENTFY");	/* Email notification */
    	token_set_y("CONFIG_RG_EVENT_LOGGING"); /* Event Logging */
	token_set_y("CONFIG_RG_8021X");
	token_set_y("CONFIG_RG_8021X_MD5");
	token_set_y("CONFIG_RG_PROXY_ARP");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_IXP425_CSR_USB");
	token_set_y("CONFIG_SIMPLE_RAMDISK");
	token_set("CONFIG_RAMDISK_SIZE", "8192");
	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_RG_PROD_IMG");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	
	/* VPN HW Acceleration */
	token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");

    }
    else if (IS_DIST("WAV54G"))
    {
	hw = "WAV54G";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_DSL");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_L2TP_CLIENT");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_ISL38XX");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_m("CONFIG_HW_BUTTONS");

	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_RG_PROD_IMG");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "eth0", NULL);
    }
    else if (IS_DIST("CX8620XR_LSP") || IS_DIST("CX8620XD_LSP"))
    {
	if (IS_DIST("CX8620XR_LSP"))
	    hw = "CX8620XR";
	else
	    hw = "CX8620XD";

	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_IPTABLES");
	token_set_m("CONFIG_BRIDGE");
	token_set_y("CONFIG_BRIDGE_UTILS");
	token_set_y("CONFIG_RG_NETTOOLS_ARP");
	token_set_m("CONFIG_ISL_SOFTMAC");
	token_set_y("CONFIG_RG_NETKIT");
	
	token_set_y("CONFIG_CX8620X_COMMON");

	token_set_y("CONFIG_HW_USB_HOST_EHCI");
    	token_set_y("CONFIG_HW_USB_STORAGE");
    }
    else if (IS_DIST("CX8620XR"))
    {
	hw = "CX8620XR";
	
	token_set_y("CONFIG_CX8620X_COMMON");
	
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
  	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
	
	dev_add_bridge("br0", DEV_IF_NET_INT, "cnx0", "ra0", NULL);
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5L");
    }
    else if (IS_DIST("CX8620XD_FILESERVER"))
    {
	hw = "CX8620XD";

	token_set_y("CONFIG_CX8620X_COMMON");
	
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_QOS");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
    	token_set_y("CONFIG_HW_USB_STORAGE");
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
 	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
	
	dev_add_bridge("br0", DEV_IF_NET_INT, "cnx0", "ra0", NULL);
    }
    else if (IS_DIST("CX8620XD_SOHO"))
    {
	hw = "CX8620XD";

	token_set_y("CONFIG_CX8620X_COMMON");

	token_set_y("PACKAGE_OPENRG_SOHO_GATEWAY");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_MAIL_FILTER");

	token_set("CONFIG_RG_SURFCONTROL_PARTNER_ID", "6002");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
    	token_set_y("CONFIG_HW_USB_STORAGE");
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
 	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5L");
	
	dev_add_bridge("br0", DEV_IF_NET_INT, "cnx0", "ra0", NULL);
    }
    else if (IS_DIST("RGLOADER_CX8620XD"))
    {
	hw = "CX8620XD";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_CX8620X_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5L");
    }
    else if (IS_DIST("COYOTE_CYBERTAN"))
    {
	hw = "COYOTE_CYBERTAN";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_SNMP");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_RG_PROD_IMG");

    }
    else if (IS_DIST("IXDP425_CYBERTAN"))
    {
	hw = "IXDP425";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_SNMP");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set_y("CONFIG_RG_PROD_IMG");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);
    }
    else if (IS_DIST("IXDP425_ATM") || IS_DIST("IXDP425_ATM_WIRELESS"))
    {
	hw = "IXDP425";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_DSL");
	if (IS_DIST("IXDP425_ATM_WIRELESS"))
	    enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_ENCRYPTION");
	if (IS_DIST("IXDP425_ATM_WIRELESS"))
	{
	    enable_module("CONFIG_HW_80211G_ISL38XX");
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "eth0", NULL);
	}

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20150");
	token_set_y("CONFIG_IXP425_ADSL_USE_MPHY");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
    }
    else if (IS_DIST("NAPA"))
    {
	hw = dist;

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
	token_set_y("CONFIG_HW_USB_HOST_OHCI");
	token_set_y("CONFIG_HW_FIREWIRE");
	token_set_y("CONFIG_HW_FIREWIRE_STORAGE");
	token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_GUI_SOHOWARE");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp1", "usb0", NULL);
    }
    /* Gemtek boards */
    else if (IS_DIST("GTWX5715") || IS_DIST("GTWX5715_WIRELESS"))
    {
	hw = "GTWX5715";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");
	token_set_y("CONFIG_RG_PROXY_ARP");
	if (IS_DIST("GTWX5715_WIRELESS")) 
	{
	    token_set_y("CONFIG_RG_8021X_RADIUS");
	    token_set_y("CONFIG_RG_WPA");
	}

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_m("CONFIG_HW_BUTTONS");
	if (IS_DIST("GTWX5715_WIRELESS")) 
	    enable_module("CONFIG_HW_80211G_ISL38XX");

	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_RG_PROD_IMG");

	/* Download image to memory before flashing
	 * Only one image section in flash, enough memory */
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");

	token_set_y("CONFIG_RG_DATE");

	if (IS_DIST("GTWX5715_WIRELESS")) 
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "eth0", NULL);
    }
    else if (IS_DIST("GTWX5715_WIRELESS_AP"))
    {
	hw = "GTWX5715";
	
	/* Foundation Module */
	token_set_y("CONFIG_OPENRG_BASIC_GATEWAY_PACKAGE");

	/* SNMP Module */
	token_set_y("CONFIG_RG_UCD_SNMP");
	token_set_y("CONFIG_RG_UCD_SNMP_V3");

	/* Firewall and Security Module */
	token_set_y("CONFIG_RG_RNAT");
	token_set_y("CONFIG_RG_FIREWALL");
	
	/* WLAN and Advanced WLAN Security Module */
	token_set_y("CONFIG_RG_8021X_FULL");
	token_set_y("CONFIG_RG_RADIUS");
	token_set_y("CONFIG_RG_WPA");
	
	/* VLAN Bridge */
	enable_module("MODULE_RG_VLAN");
    }
    else if (IS_DIST("HG21"))
    {
	hw = "HG21";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_ISL38XX");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_RG_INSMOD_SILENT");
	token_set("CONFIG_JFFS2_FS", "m");
	token_set_y("CONFIG_IXP425_JFFS2_WORKAROUND");
	token_set_y("CONFIG_IXP425_CSR_HSS");
	token_set("CONFIG_IXP425_CODELETS", "m");
	token_set("CONFIG_IXP425_CODELET_HSS", "m");
	token_set_y("CONFIG_GUI_WELLTECH");
	token_set_y("CONFIG_GUI_RG");
	token_set_y("CONFIG_RG_SYSLOG_REMOTE");
	token_set_y("CONFIG_RG_PROD_IMG");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "eth0", NULL);
    }
    else if (IS_DIST("IXDP425_FRG"))
    {
	hw = "IXDP425";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20150");
	token_set_y("CONFIG_IXP425_ADSL_USE_MPHY");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");	

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);
    }
    else if (IS_DIST("IXDP425_TMT"))
    {
	hw = "IXDP425_TMT";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_m("CONFIG_HW_BUTTONS");

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_RG_PROD_IMG");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
    }
    else if (IS_DIST("WADB100G_RGLOADER"))
    {
    	hw = "WADB100G";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_BCM963XX_BOOTSTRAP");
	token_set_m("CONFIG_RG_KRGLDR");

	/* Create flash image since it rgloader does not reside in section 0.
	 * This flash image (openrg.prod) is burned to address 0, instead of 
	 * rgloader.img.
	 */
	token_set_y("CONFIG_RG_PROD_IMG");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_MIPSEB");
    }
    else if (IS_DIST("WADB100G"))
    {
    	hw = "WADB100G";
	os = "LINUX_24";
	
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_MAIL_FILTER");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_DSL");
	
	token_set_y("CONFIG_RG_IPP");
	token_set_y("CONFIG_RG_DATE");
	
	/* Include automatic daylight saving time calculation */
	
	token_set_y("CONFIG_RG_TZ_FULL");
	token_set("CONFIG_RG_TZ_YEARS", "5");
	
	/* All WBM pages expire with session */
	
	token_set_y("CONFIG_WBM_PAGES_EXPIRE");
	
	/* OpenRG HW support */
	
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_USB_RNDIS");
	token_set_m("CONFIG_HW_BUTTONS");
        token_set_y("CONFIG_HW_LEDS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "bcm0", "bcm1", "usb0", NULL);
    }
    else if (IS_DIST("MPC8272ADS_LSP"))
    {
	/* Hardware */
	hw = "MPC8272ADS";
	os = "LINUX_24";

	token_set_y("CONFIG_MPC8272ADS");

	/* Software */
	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_SIMPLE_RAMDISK");
	token_set_y("CONFIG_LSP_FLASH_LAYOUT");
	token_set("CONFIG_RAMDISK_SIZE", "8192");
    }
    else if (IS_DIST("MPC8272ADS") || IS_DIST("MPC8272ADS_EEPRO1000"))
    {
	hw = "MPC8272ADS";
	os = "LINUX_24";
	
	token_set_y("CONFIG_RG_DATE");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_MAIL_FILTER");
	enable_module("MODULE_RG_URL_FILTERING");
	token_set("CONFIG_RG_SURFCONTROL_PARTNER_ID", "6005");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_DSLHOME");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");

	/* Include automatic daylight saving time calculation */
	
	token_set_y("CONFIG_RG_TZ_FULL");
	token_set("CONFIG_RG_TZ_YEARS", "5");

	/* HW Configuration Section */
	
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	
        if (IS_DIST("MPC8272ADS_EEPRO1000"))
	{
	    token_set_y("CONFIG_HW_ETH_EEPRO1000");
	}

	/* Ralink RT2560 */
	
	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");

	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	dev_add_bridge("br0", DEV_IF_NET_INT, "eth1", "ra0", NULL);

	token_set_y("CONFIG_HW_USB_HOST_EHCI");
	token_set_y("CONFIG_HW_USB_HOST_OHCI");
	token_set_y("CONFIG_HW_USB_HOST_UHCI");
	token_set_y("CONFIG_HW_USB_STORAGE");
	token_set_y("CONFIG_GENERIC_ISA_DMA"); /* needed for scsi driver USB
						* storage */
        token_set("CONFIG_RG_JPKG_DIST", "JPKG_PPC");
    }
    else if (IS_DIST("CENTAUR_LSP"))
    {
	hw = "CENTAUR";
 	os = "LINUX_24";
 
 	token_set_y("CONFIG_KS8695_COMMON");
 
 	/* Software */
 	token_set_y("CONFIG_LSP_DIST");
 	token_set_y("CONFIG_SIMPLE_RAMDISK");
 	token_set_y("CONFIG_LSP_FLASH_LAYOUT");
 	token_set("CONFIG_RAMDISK_SIZE", "1024");
 	token_set_y("CONFIG_HW_ETH_WAN");
 	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("CENTAUR") || IS_DIST("CENTAUR_VGW"))
    {
 	hw = "CENTAUR";
 	os = "LINUX_24";
 
 	enable_module("MODULE_RG_FOUNDATION");
 	enable_module("MODULE_RG_ADVANCED_ROUTING");
 	enable_module("MODULE_RG_VLAN");
 	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
   	enable_module("MODULE_RG_PPP");
 	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
 	enable_module("MODULE_RG_URL_FILTERING");
 	enable_module("MODULE_RG_IPSEC");
 	enable_module("MODULE_RG_PPTP");
 	enable_module("MODULE_RG_L2TP_CLIENT");
 	enable_module("MODULE_RG_ZERO_CONFIGURATION");
 	enable_module("MODULE_RG_SNMP");
 	enable_module("MODULE_RG_IPV6");
 	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_MAIL_FILTER");
 
 	/* HW Configuration Section */
 	token_set_y("CONFIG_HW_ETH_WAN");
 	token_set_y("CONFIG_HW_ETH_LAN");
 	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
 
 	token_set_y("CONFIG_80211G_AP_ADVANCED");
 	token_set_y("CONFIG_RG_WPA_WBM");
 	token_set_y("CONFIG_RG_RADIUS_WBM");
 	token_set_y("CONFIG_RG_RADIUS_IN_CONN");
 	token_set_y("CONFIG_RG_8021X_WBM");
 	
 	token_set("CONFIG_RG_SSID_NAME", "Centaur");
 	dev_add_bridge("br0", DEV_IF_NET_INT, "eth1", "ra0", NULL);

	if (IS_DIST("CENTAUR_VGW"))
	{
	    enable_module("MODULE_RG_VOIP_OSIP");
	    /* Due to flash size limitations we will not include RV stack */
#if 0
	    enable_module("MODULE_RG_VOIP_RV_MGCP");
	    enable_module("MODULE_RG_VOIP_RV_H323");
#endif
	    token_set_y("CONFIG_HW_DSP");
	}
 
 	/* Software */
 	token_set_y("CONFIG_KS8695_COMMON");
    }
    else if (IS_DIST("RGLOADER_CENTAUR"))
    {
 	hw = "CENTAUR";
 	os = "LINUX_24";
 
 	token_set_y("CONFIG_RG_RGLOADER");
 
 	token_set_y("CONFIG_KS8695_COMMON");
 	token_set_y("CONFIG_SIMPLE_RAMDISK");
 	token_set("CONFIG_RAMDISK_SIZE", "4096");
 	token_set_y("CONFIG_PROC_FS");
 	token_set_y("CONFIG_EXT2_FS");
 	token_set_y("CONFIG_HW_ETH_WAN");
 	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("IXDP425_LSP"))
    {
	/* Hardware */
	hw = "IXDP425";

        token_set_y("CONFIG_IXP425_COMMON_LSP");
        token_set("CONFIG_IXDP425_KGDB_UART", "1");

	/* ADSL Chip Alcatel 20150 on board */
	token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20150");
	token_set_y("CONFIG_IXP425_ADSL_USE_MPHY");

	/* EEPROM */
	token_set("CONFIG_PCF8594C2", "m");

	/* IXP425 Eth driver module */
	token_set("CONFIG_IXP425_ETH", "m");

        /* Flash chip */
	token_set_y("CONFIG_IXP425_FLASH_E28F128J3");

	/* Software */
	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_SIMPLE_RAMDISK");
	token_set_y("CONFIG_LSP_FLASH_LAYOUT");
    }
    else if (IS_DIST("COYOTE_CAVIUM"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);
    }
    else if (IS_DIST("COYOTE_ACTIONTEC_OSIP"))
    {
	hw = "COYOTE";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_VOIP_OSIP");
	enable_module("MODULE_RG_PPP");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_ISL38XX");
	enable_module("CONFIG_HW_DSP");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "eth0", NULL);
    }
    else if (IS_DIST("COYOTE") || IS_DIST("COYOTE_FULLSOURCE") ||
	IS_DIST("COYOTE_ATM"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_ARM_24_FAST_MODULES");

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_PRINTSERVER");

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5B");

	enable_module("MODULE_RG_VOIP_OSIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	    
	enable_module("MODULE_RG_MAIL_FILTER");
	
	/* HW Configuration Section */
	if (IS_DIST("COYOTE_ATM"))
	{
	    enable_module("MODULE_RG_DSL");
	    token_set_y("CONFIG_HW_DSL_WAN");
	}
	else
	    token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
	enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");
	enable_module("CONFIG_HW_80211G_RALINK_RT2560");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "ra0", NULL);

	if (IS_DIST("COYOTE_FULLSOURCE"))
	{
	    token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	    token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");
	    enable_module("MODULE_RG_ADVANCED_OFFICE_SERVERS");
	}
    }
    else if (IS_DIST("COYOTE_EICON"))
    {
	hw = "COYOTE";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_DSL");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_SNMP");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP_CLIENT");
        token_set_y("CONFIG_RG_EVENT_LOGGING");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211G_ISL38XX");
        enable_module("CONFIG_HW_ENCRYPTION");

	token_set_y("CONFIG_RG_PROD_IMG");
	token_set_y("CONFIG_GLIBC");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);
    }
    else if (IS_DIST("COYOTE_BKTEL"))
    {
	hw = "COYOTE_WIRELESS";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_VLAN");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_URL_FILTERING");
	token_set_m("CONFIG_RG_PPPOE_RELAY"); /* PPPoE Relay */
	token_set_y("CONFIG_RG_STATIC_ROUTE"); /* Static Routing */

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211G_ISL38XX");
	enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
	token_set_y("CONFIG_HW_USB_HOST_OHCI");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);
	
	token_set_y("CONFIG_RG_PROD_IMG");
	token_set("CONFIG_RG_SURFCONTROL_PARTNER_ID", "6005");
    }
    else if (IS_DIST("COYOTE_POSSIO"))
    {
	hw = "COYOTE_WIRELESS";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_VLAN");
	token_set_y("CONFIG_RG_PRINT_SERVER");
	enable_module("CONFIG_RG_LPD");
	token_set_y("CONFIG_RG_IPP");
	
	/* HW Configuration Section */
  	token_set_y("CONFIG_HW_ETH_WAN");
  	token_set_y("CONFIG_HW_ETH_LAN");
        /* enable_module("CONFIG_HW_80211G_ISL38XX"); */
	enable_module("CONFIG_HW_80211G_RALINK_RT2560");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_y("CONFIG_HW_USB_STORAGE");
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
	token_set_y("CONFIG_HW_USB_HOST_OHCI");
	token_set_y("CONFIG_HW_BUTTONS");
	token_set_y("CONFIG_HW_LEDS");
	token_set_y("CONFIG_RG_PPPOS");
    	token_set_y("CONFIG_RG_DHCPR");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "ra0", NULL);
    }
    else if (IS_DIST("COYOTE_SRI"))
    {
	hw = "COYOTE_WIRELESS";

	token_set_y("CONFIG_RG_SMB");

  	/* PACKAGE_OPENRG_VPN_GATEWAY minus MODULE_RG_WLAN_AND_ADVANCED_WLAN */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_URL_FILTERING");
	token_set("CONFIG_RG_SURFCONTROL_PARTNER_ID", "6003");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_RALINK_RT2560");
	token_set_y("CONFIG_HW_ENCRYPTION");
	token_set_y("CONFIG_HW_BUTTONS");
	token_set_y("CONFIG_HW_LEDS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "ra0", NULL);
    }
    else if (IS_DIST("COYOTE_ADEPT"))
    {
	hw = "COYOTE";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_DSL");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

  	token_set_y("CONFIG_HW_DSL_WAN");
  	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_ENCRYPTION");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211G_RALINK_RT2560");
	enable_module("CONFIG_HW_DSP");

	dev_add_bridge("br0", DEV_IF_NET_INT,"ixp0", "ra0", "usb0", NULL);
    }
    else if (IS_DIST("COYOTE_EXAVERA"))
    {
	hw = "COYOTE_WIRELESS";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_VLAN");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_VOIP_RV_SIP");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211G_ISL38XX");
	enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);
	
	token_set_y("CONFIG_RG_PROD_IMG");
    }
    else if (IS_DIST("COYOTE_JSTREAM"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_TFTP_UPGRADE");
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_URL_FILTERING");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_m("CONFIG_HW_BUTTONS");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", NULL);
	
	token_set("CONFIG_RG_SURFCONTROL_PARTNER_ID", "6005");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");

	/* SSI pages - requested specifically */
	token_set_y("CONFIG_RG_SSI_PAGES");

	token_set_y("CONFIG_GLIBC");
    }
    else if (IS_DIST("COYOTE_DIALFACE"))
    {
	hw = "COYOTE_ATM_WIRELESS";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_DSL");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
    	token_set_y("CONFIG_RG_EVENT_LOGGING"); /* Event Logging */

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211G_RALINK_RT2560");
	token_set("CONFIG_RG_SSID_NAME", "openrg");
	enable_module("CONFIG_HW_DSP");

	token_set_y("CONFIG_RG_PROD_IMG");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "ra0", NULL);
    }
    else if (IS_DIST("MONTEJADE") || IS_DIST("MONTEJADE_AGN100") ||
	IS_DIST("MONTEJADE_ATM") || IS_DIST("MONTEJADE_SERVER"))
    {
	hw = "MONTEJADE";

	token_set_y("CONFIG_RG_SMB");

	/* Notice: this config interfere with kernel debugger. Disable it while
	 * debugging.
	 */
	token_set_y("CONFIG_ARM_24_FAST_MODULES");
 	
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");
        enable_module("MODULE_RG_VOIP_OSIP");
        enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_MAIL_FILTER");
	token_set("CONFIG_RG_SURFCONTROL_PARTNER_ID", "6005");

	/* HW Configuration Section */
	token_set_m("CONFIG_HW_BUTTONS");
	if (IS_DIST("MONTEJADE_ATM"))
	{
	    enable_module("MODULE_RG_DSL");
	    token_set_y("CONFIG_HW_DSL_WAN");
	}
	else
	    token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
	token_set_y("CONFIG_HW_USB_HOST_OHCI");
	token_set_y("CONFIG_HW_USB_STORAGE");
        enable_module("CONFIG_HW_ENCRYPTION");

	/* DSR support */
	enable_module("CONFIG_HW_DSP");

	if (IS_DIST("MONTEJADE_AGN100"))
	{
	    token_set_y("CONFIG_HW_80211N_AIRGO_AGN100");
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "wlan0",
		NULL);
	}
	else
	{
	    token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "ra0", NULL);
	}
	if (IS_DIST("MONTEJADE_SERVER"))
	{
	    enable_module("MODULE_RG_ADVANCED_OFFICE_SERVERS");
	    token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "4194304");
	}
    }
    else if (IS_DIST("PCBOX_EEP_EEP_EICON") || IS_DIST("PCBOX_RTL_RTL_EICON"))
    {
	if (IS_DIST("PCBOX_EEP_EEP_EICON"))
	    hw = "PCBOX_EEP_EEP";
	else
	    hw = "ALLWELL_RTL_RTL";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_IPSEC");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_RG_PROD_IMG");
	token_set_y("CONFIG_GLIBC");
    }
    else if (IS_DIST("COYOTE_LSP"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_GLIBC");
	token_set_y("CONFIG_SIMPLE_RAMDISK");
	token_set_y("CONFIG_LSP_FLASH_LAYOUT");
	token_set_y("CONFIG_IXP425_COMMON_LSP");
    }
    else if (IS_DIST("COYOTE_EEP"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_EEPRO100_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eep0", NULL);
    }
    else if (IS_DIST("COYOTE_ALL"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	token_set_y("CONFIG_RG_PRINT_SERVER");
	enable_module("CONFIG_RG_LPD");
	token_set_y("CONFIG_RG_IPP");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("MODULE_RG_DSL");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211G_ISL38XX");
	enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");

	dev_add_bridge("br0", DEV_IF_NET_INT, NULL);
    }
    else if (IS_DIST("COYOTE_ATM_WIRELESS_ISL"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	token_set_y("CONFIG_RG_PRINT_SERVER");
	enable_module("CONFIG_RG_LPD");
	token_set_y("CONFIG_RG_IPP");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("MODULE_RG_DSL");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211G_ISL38XX");
	enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);
    }
    else if (IS_DIST("COYOTE_FRANCE_TELECOM"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	token_set_y("CONFIG_RG_PRINT_SERVER");
	enable_module("CONFIG_RG_LPD");
	token_set_y("CONFIG_RG_IPP");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("MODULE_RG_DSL");
	enable_module("MODULE_RG_PRINTSERVER");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");

	/* HW Configuration Section */
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "32"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_IXP425_NUMBER_OF_FLASH_CHIPS", "2");
	token_set_y("CONFIG_MTD_CONCAT");
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211G_ISL38XX");
	enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);
    }
    else if (IS_DIST("COYOTE_ATM_WIRELESS"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("MODULE_RG_DSL");
	token_set_y("CONFIG_RG_PRINT_SERVER");
	enable_module("CONFIG_RG_LPD");
	token_set_y("CONFIG_RG_IPP");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211B_PRISM2");
	enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);
    }
    else if (IS_DIST("COYOTE_CAC"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_DSL");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	token_set_y("CONFIG_AUTO_LEARN_DNS");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");

	/* When removing IPSec module you MUST remove the HW ENCRYPTION config
	 * from the HW section aswell */
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_L2TP_CLIENT");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_HSS_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_USB_HOST_UHCI");
	token_set_y("CONFIG_HW_USB_HOST_OHCI");
	enable_module("CONFIG_HW_ENCRYPTION");
	enable_module("CONFIG_HW_DSP");

	token_set_y("CONFIG_RG_RADIUS_LOGIN_AUTH");
    }
    else if (IS_DIST("RGLOADER_BRUCE"))
    {
	hw = "BRUCE";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("BRUCE"))
    {
	hw = "BRUCE";

	/* Modules */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_OFFICE_SERVERS");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_80211G_ISL38XX");
	enable_module("CONFIG_HW_ENCRYPTION");
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
	token_set_y("CONFIG_HW_USB_HOST_OHCI");
	token_set_y("CONFIG_HW_USB_STORAGE");
	token_set_y("CONFIG_HW_IDE");
	token_set_m("CONFIG_HW_BUTTONS");
	token_set_m("CONFIG_HW_LEDS");
        token_set_y("CONFIG_HW_CLOCK");
        token_set_y("CONFIG_HW_I2C");
        token_set_y("CONFIG_HW_ENV_MONITOR");

	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp1", "eth0", NULL);
    }
    else if (IS_DIST("COYOTE_JABIL"))
    {
	hw = "COYOTE";

	/* Modules */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_OFFICE_SERVERS");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_80211G_ISL38XX");
	enable_module("CONFIG_HW_ENCRYPTION");
	token_set_y("CONFIG_HW_IDE");

	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "usb0", "eth0", NULL);
    }
    /* REDBOOT distributions for IXP425 based boards */
    else if (IS_DIST("REDBOOT_RICHFIELD"))
    {
	hw = "RICHFIELD";
	os = "ECOS";
	token_set("DIST_TYPE", "BOOTLDR");
    }
    else if (IS_DIST("REDBOOT_MATECUMBE"))
    {
	hw = "MATECUMBE";
	os = "ECOS";
	token_set("DIST_TYPE", "BOOTLDR");
    }
    else if (IS_DIST("REDBOOT_COYOTE"))
    {
	hw = "COYOTE";
	os = "ECOS";
	token_set("DIST_TYPE", "BOOTLDR");
    }
    else if (IS_DIST("REDBOOT_NAPA"))
    {
	hw = "NAPA";
	os = "ECOS";
	token_set("DIST_TYPE", "BOOTLDR");
    }
    else if (IS_DIST("RGLOADER_IXDP425") ||
	IS_DIST("RGLOADER_IXDP425_FULLSOURCE") ||
	IS_DIST("RGLOADER_IXDP425_16MB") || IS_DIST("RGLOADER_IXDP425_LSP"))
    {
	hw = "IXDP425";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set("CONFIG_IXDP425_KGDB_UART", "1");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");	

	if (IS_DIST("RGLOADER_IXDP425_16MB"))
	    token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16");

	if (IS_DIST("RGLOADER_IXDP425_LSP"))
	    token_set_y("CONFIG_LSP_FLASH_LAYOUT");
    }
    else if (IS_DIST("RGLOADER_MATECUMBE"))
    {
	hw = "MATECUMBE";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("RGLOADER_NAPA"))
    {
	hw = "NAPA";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");
	token_set_y("CONFIG_IXP425_POST");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("RGLOADER_COYOTE"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("RGLOADER_COYOTE_64MB_RAM"))
    {
	hw = "COYOTE";
	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5B");
    }
    else if (IS_DIST("RGLOADER_COYOTE_64MB_RAM_32MB_FLASH"))
    {
	hw = "COYOTE";
	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");
	token_set_y("CONFIG_MTD_CONCAT");

	/* HW Configuration Section */
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "32"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_IXP425_NUMBER_OF_FLASH_CHIPS", "2");
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("RGLOADER_COYOTE_LSP"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");
	token_set_y("CONFIG_LSP_FLASH_LAYOUT");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("RGLOADER_COYOTE_FULLSOURCE"))
    {
	hw = "COYOTE";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
    }
    else if (IS_DIST("RGLOADER_MONTEJADE"))
    {
	hw = "MONTEJADE";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5B");
    }
    else if (IS_DIST("RGLOADER_HG21"))
    {
	hw = "HG21";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
    }
    else if (IS_DIST("RGLOADER_BAMBOO"))
    {	
	hw = "BAMBOO";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
    }
    else if (IS_DIST("RGLOADER_GTWX5715"))
    {
	hw = "GTWX5715";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");

	/* Disable VLAN switch on production boards */
	token_set_y("CONFIG_WX5715_VLAN_SWITCH_DISABLE");
    }
    else if (IS_DIST("RGLOADER_ALLWELL_RTL_RTL"))
    {
	hw = "ALLWELL_RTL_RTL";

	token_set_y("CONFIG_RG_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
    }
    else if (IS_DIST("RGLOADER_ALLWELL_RTL_EEP"))
    {
	hw = "ALLWELL_RTL_EEP";

	token_set_y("CONFIG_RG_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
    }
    else if (IS_DIST("RGLOADER_ALLWELL_EEP_EEP"))
    {
	hw = "PCBOX_EEP_EEP";

	token_set_y("CONFIG_RG_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
    }
    else if (IS_DIST("RGLOADER_WAV54G"))
    {
	hw = "WAV54G";

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_m("CONFIG_HW_BUTTONS");
    }
    else if (IS_DIST("WRT55AG") || IS_DIST("ATHEROS_AR531X_AG_VX") ||
	IS_DIST("WRT55AG_INT"))
    {
	os = "VXWORKS";

	if (IS_DIST("WRT55AG"))
	    hw = "AR531X_WRT55AG";
	else if (IS_DIST("ATHEROS_AR531X_AG_VX"))
	    hw = "AR531X_AG";
	else
	    hw = "AR531X_G";
		
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
        enable_module("MODULE_RG_URL_FILTERING");

	/* OpenRG HW support */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_80211G_AR531X");
	if (IS_DIST("WRT55AG") || IS_DIST("ATHEROS_AR531X_AG_VX"))
	    enable_module("CONFIG_HW_80211A_AR531X");
	token_set_m("CONFIG_HW_BUTTONS");

	token_set_y("CONFIG_RG_SSI_PAGES");
	token_set_y("CONFIG_RG_LAN_BRIDGE_CONST");
	token_set_y("CONFIG_RG_RMT_MNG");
	token_set_y("CONFIG_RG_STATIC_ROUTE"); /* Static Routing */
	token_set_y("CONFIG_RG_ENTFY"); /* Email notification */
    	token_set_y("CONFIG_RG_EVENT_LOGGING");
	token_set("CONFIG_SDRAM_SIZE", "16");
	token_set_y("CONFIG_RG_PROD_IMG");

	token_set_y("CONFIG_RG_WLAN_STA_STATISTICS_WBM");

	/* WLAN */
	token_set_y("CONFIG_80211G_AP_ADVANCED");
	token_set_y("CONFIG_RG_WPA_WBM");
	token_set_y("CONFIG_RG_RADIUS_WBM");
	token_set_y("CONFIG_RG_RADIUS_IN_CONN");
	token_set_y("CONFIG_RG_8021X_WBM");

	if (!IS_DIST("WRT55AG_INT"))
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ae0", "vp0", "vp256", NULL);
	else
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ae2", "vp256", NULL);
    }
    else if (IS_DIST("AR2313_LIZZY") || IS_DIST("BOCHS_LIZZY"))
    {
	os = "VXWORKS";

	/* This distribution is FOUNDATION_CORE plus many features */
	token_set_y("CONFIG_RG_FOUNDATION_CORE");

    	/* DHCP Server */
	token_set_y("CONFIG_RG_DHCPS");

	/* Telnet Server */
	token_set_y("CONFIG_RG_TELNETS");

	/* Bridging */
	token_set_y("CONFIG_RG_BRIDGE");
	
	/* NAT/NAPT */	
	token_set_y("CONFIG_RG_NAT");

	/* ALG support */
	token_set_y("CONFIG_RG_ALG");
	token_set_y("CONFIG_RG_ALG_SIP");
	token_set_y("CONFIG_RG_ALG_H323");
	token_set_y("CONFIG_RG_ALG_AIM");
	token_set_y("CONFIG_RG_ALG_MSNMS");
	token_set_y("CONFIG_RG_ALG_PPTP");
	token_set_y("CONFIG_RG_ALG_IPSEC");
	token_set_y("CONFIG_RG_ALG_L2TP");
	token_set_y("CONFIG_RG_ALG_ICMP");
	token_set_y("CONFIG_RG_ALG_PORT_TRIGGER");
	token_set_y("CONFIG_RG_ALG_FTP");
	token_set_y("CONFIG_RG_ALG_RTSP");

	/* Firewall */
	token_set_y("CONFIG_RG_MSS");
	
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");

	/* Vlan */
	enable_module("MODULE_RG_VLAN");

	/* Additional features */
    	token_set_y("CONFIG_RG_DHCPR");
	token_set_y("CONFIG_RG_URL_KEYWORD_FILTER");

	if (IS_DIST("AR2313_LIZZY"))
	{
	    /* WLAN */
	    enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	    token_set_y("CONFIG_RG_RADIUS_IN_CONN");
	    token_set_y("CONFIG_RG_WLAN_REPEATING");
	    token_set_y("CONFIG_RG_WDS_CONN_NOTIFIER");
	    token_set("CONFIG_RG_SSID_NAME", "wireless");
	}

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	if (IS_DIST("AR2313_LIZZY"))
	{
	    enable_module("CONFIG_HW_80211G_AR531X");
	    token_set_y("CONFIG_HW_BUTTONS");
	}

	token_set_y("CONFIG_RG_SSI_PAGES");
	token_set_y("CONFIG_RG_LAN_BRIDGE_CONST");
	token_set_y("CONFIG_RG_RMT_MNG");
	token_set_y("CONFIG_RG_STATIC_ROUTE"); /* Static Routing */

	/* Email notification */
	token_set_y("CONFIG_RG_ENTFY");

	/* Event Logging */
    	token_set_y("CONFIG_RG_EVENT_LOGGING");

	token_set("CONFIG_SDRAM_SIZE", "8");

	/* Statistics control */
	token_set_y("CONFIG_RG_STATISTICS_CTRL_WBM");
	token_set_y("CONFIG_RG_WLAN_STA_STATISTICS_WBM");

	if (IS_DIST("AR2313_LIZZY"))
	{
	    token_set_y("CONFIG_RG_PROD_IMG");

	    hw = "AR531X_G";

	    dev_add_bridge("br0", DEV_IF_NET_INT, "ae2", "vp256", NULL);
	}
	else /* BOCHS_LIZZY */
	{
	    hw = "I386_BOCHS";

	    dev_add_bridge("br0", DEV_IF_NET_INT, "ene0", "ene1", NULL);
	}
    }
    else if (IS_DIST("AR2313_ZIPPY") || IS_DIST("BOCHS_ZIPPY"))
    {
	os = "VXWORKS";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");

	/* Additional features */
    	token_set_y("CONFIG_RG_DHCPR");
	token_set_y("CONFIG_RG_URL_KEYWORD_FILTER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	if (IS_DIST("AR2313_ZIPPY"))
	{
	    enable_module("CONFIG_HW_80211G_AR531X");
	    token_set_y("CONFIG_HW_BUTTONS");
	}

	token_set_y("CONFIG_RG_SSI_PAGES");
	token_set_y("CONFIG_RG_LAN_BRIDGE_CONST");

	if (IS_DIST("AR2313_ZIPPY"))
	{
	    enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	    token_set_y("CONFIG_RG_RADIUS_IN_CONN");
	    token_set_y("CONFIG_AR531X_DEBUG");
	    token_set_y("CONFIG_RG_PROD_IMG");
	}

	token_set("CONFIG_SDRAM_SIZE", "16");

	/* Statistics control */
	token_set_y("CONFIG_RG_STATISTICS_CTRL_WBM");
	token_set_y("CONFIG_RG_WLAN_STA_STATISTICS_WBM");

	if (IS_DIST("AR2313_ZIPPY"))
	{
	    hw = "AR531X_G";
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ae2", "vp256", NULL);
	} 
	else /* BOCHS_ZIPPY */
	{
	    hw = "I386_BOCHS";
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ene0", "ene1", NULL);
	}
    }
    else if (IS_DIST("ATHEROS_AR531X_VX"))
    {
	os = "VXWORKS";
	hw = "AR531X_G";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	token_set_y("CONFIG_RG_TFTP_UPGRADE");
	token_set_y("CONFIG_RG_TFTP_SERVER_PASSWORD");
	token_set_y("CONFIG_RG_ENTFY");	/* Email notification */
    	token_set_y("CONFIG_RG_EVENT_LOGGING"); /* Event Logging */
	token_set_y("CONFIG_RG_URL_KEYWORD_FILTER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_80211G_AR531X");
	token_set_y("CONFIG_HW_BUTTONS");

	token_set_y("CONFIG_RG_RADIUS_IN_CONN");
	token_set_y("CONFIG_RG_SSI_PAGES");
	token_set_y("CONFIG_AR531X_DEBUG");
	token_set_y("CONFIG_RG_LAN_BRIDGE_CONST");
	token_set_y("CONFIG_RG_PROD_IMG");
	token_set("CONFIG_SDRAM_SIZE", "16");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ae2", "vp256", NULL);
    }
    else if (IS_DIST("WRT108G_VX"))
    {
	os = "VXWORKS";
	hw = "WRT108G";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	token_set_y("CONFIG_RG_TFTP_UPGRADE");
	token_set_y("CONFIG_RG_TFTP_SERVER_PASSWORD");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_80211G_AR531X");
	token_set_y("CONFIG_HW_BUTTONS");

	token_set_y("CONFIG_RG_RADIUS_IN_CONN");
	token_set_y("CONFIG_GUI_LINKSYS");
	token_set_y("CONFIG_RG_LAN_BRIDGE_CONST");

	token_set_y("CONFIG_RG_RGLOADER_CLI_CMD");
	token_set_y("CONFIG_RG_PROD_IMG");
	dev_add_bridge("br0", DEV_IF_NET_INT, "ae2", "ar1", NULL);

	token_set("CONFIG_SDRAM_SIZE", "16");

	/* Features not ready */
#if 0
	token_set_y("CONFIG_RG_IGMP_PROXY");
	
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");
#endif
    }
    else if (IS_DIST("TI_404_VX_EVAL") || IS_DIST("T_TI_404_VX") ||
    	IS_DIST("ARRIS_TI_404_VX"))
    {
	os = "VXWORKS";
	hw = "TI_404";

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_SNMP");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_CABLE_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	if (IS_DIST("ARRIS_TI_404_VX"))
	    token_set_y("CONFIG_GUI_ARRIS");
    }
    else if (IS_DIST("T_TI_404_VX_CH") || IS_DIST("ARRIS_TI_404_VX_CH"))
    {
	os = "VXWORKS";
	hw = "TI_404";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_CABLEHOME");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
    	token_set_y("CONFIG_RG_EVENT_LOGGING"); /* Event Logging */

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_CABLE_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	if (IS_DIST("ARRIS_TI_404_VX_CH"))
	    token_set_y("CONFIG_GUI_ARRIS");

	if (IS_DIST("SMC_TI_404_VX_CH"))
	{
	    enable_module("MODULE_RG_ZERO_CONFIGURATION");
	    enable_module("MODULE_RG_ADVANCED_ROUTING");
	}

	dev_add_bridge("br0", DEV_IF_NET_INT, "cbl0", "lan0", NULL);
    }
    else if (IS_DIST("TI_TNETC440_VX_CH") || IS_DIST("HITRON_TNETC440_VX_CH") ||
	IS_DIST("HITRON_TNETC440_VX"))
    {
	os = "VXWORKS";
	hw = "TI_404";

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
    	token_set_y("CONFIG_RG_EVENT_LOGGING");	/* Event Logging */
	if (!IS_DIST("HITRON_TNETC440_VX"))
	    enable_module("MODULE_RG_CABLEHOME");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_CABLE_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	dev_add_bridge("br0", DEV_IF_NET_INT, "cbl0", "lan0", NULL);

	if (IS_DIST("HITRON_TNETC440_VX_CH") || IS_DIST("HITRON_TNETC440_VX"))
	    token_set_y("CONFIG_HITRON_BSP");
    }
    else if (IS_DIST("TI_404_VX_CH_EVAL"))
    {
	os = "VXWORKS";
	hw = "TI_404";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_CABLEHOME");
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_CABLE_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set("CONFIG_RG_SURFCONTROL_PARTNER_ID", "6005");
	token_set_y("CONFIG_RG_PROD_IMG");

	dev_add_bridge("br0", DEV_IF_NET_INT, "cbl0", "lan0", NULL);
    }
    else if (IS_DIST("TI_WPA_VX"))
    {
	os = "VXWORKS";
	hw = "TI_TNETWA100";
	token_set_y("CONFIG_RG_WPA");
	token_set_y("CONFIG_RG_8021X_RADIUS");
	token_set_y("CONFIG_RG_OPENSSL");
    }
    else if (IS_DIST("I386_BOCHS_VX"))
    {
	os = "VXWORKS";
	hw = "I386_BOCHS";

	token_set_y("CONFIG_RG_SMB");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	dev_add_bridge("br0", DEV_IF_NET_INT, "ene0", "ene1", NULL);
    }
    else if (IS_DIST("ALLWELL_RTL_RTL_VALGRIND"))
    {
	hw = "ALLWELL_RTL_RTL";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_ADVANCED_ROUTING");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_RG_PROD_IMG");
	token_set_y("CONFIG_VALGRIND");
    }
    else if (IS_DIST("ALLWELL_RTL_RTL_ISL38XX"))
    {
	hw = "ALLWELL_RTL_RTL_ISL38XX";

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
        enable_module("CONFIG_HW_80211G_ISL38XX");

	token_set_y("CONFIG_RG_PROD_IMG");

	dev_add_bridge("br0", DEV_IF_NET_INT, "rtl1", "eth0", NULL);
    }
    else if (IS_DIST("RGLOADER_UML"))
    {
	hw = "UML";
	os = "LINUX_24";

	token_set_y("CONFIG_RG_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");
    }
    else if (IS_DIST("RGLOADER_MI424WR") || IS_DIST("RGLOADER_VI414WG") ||
	IS_DIST("RGLOADER_RI408WR") || IS_DIST("RGLOADER_KI414WG") ||
	IS_DIST("RGLOADER_MI524WR"))
    {
	hw = dist + strlen("RGLOADER_");

	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_LEDS");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	token_set("RG_PROD_STR", "Actiontec Home Router");

	token_set("CONFIG_RG_RMT_UPD_DIST", hw);
	token_set_y("CONFIG_RG_TFTP_UPGRADE");
	token_set_y("CONFIG_HW_BUTTONS");
    }
    else if (IS_DIST("RGLOADER_BA214WG"))
    {
	hw = dist + strlen("RGLOADER_");

	token_set_y("CONFIG_RG_RGLOADER");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_LEDS");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	/* Override HW settings to save RAM for network boot feature */
	token_set("CONFIG_SDRAM_SIZE", "16");
	/* Enable boot of image from network */
	token_set_y("CONFIG_RG_NETWORK_BOOT");

	token_set("RG_PROD_STR", "Actiontec Home Router");

	token_set("CONFIG_RG_RMT_UPD_DIST", hw);
	token_set_y("CONFIG_RG_TFTP_UPGRADE");
	token_set_y("CONFIG_HW_BUTTONS");
    }
    else if (IS_DIST("JPKG_SRC"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"src\"");
	token_set_y("CONFIG_RG_JPKG");
	token_set_y("CONFIG_RG_JPKG_SRC");
        token_set_y("CONFIG_RG_UML");
	token_set_y("CONFIG_IXP425_COMMON");
	token_set_y("CONFIG_IXP425_DSR");
	
	set_jpkg_modules();
	enable_module("MODULE_RG_ADVANCED_OFFICE_SERVERS");

	token_set_y("CONFIG_RG_AUDIO_MGT");
	token_set_y("CONFIG_IXP425_COMMON_RG");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");
	/* XXX: voip-osip-gpl is broken in jpkg_defines.c
	 * enable_module("MODULE_RG_VOIP_OSIP");
	 */
	enable_module("MODULE_RG_FILESERVER");
	token_set_y("CONFIG_RG_PRINT_SERVER");
	enable_module("CONFIG_RG_LPD");
	token_set_y("CONFIG_RG_IPP");
        enable_module("CONFIG_HW_USB_RNDIS");
	enable_module("CONFIG_HW_80211G_ISL38XX");
	token_set_m("CONFIG_ISL38XX");
	token_set_m("CONFIG_RG_USB_SLAVE");
	token_set_y("CONFIG_IXP425_CSR_USB");
        enable_module("CONFIG_HW_DSP");
        enable_module("CONFIG_HW_ENCRYPTION");
	token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	token_set_y("CONFIG_IPSEC_ENC_AES");
	token_set_m("CONFIG_RALINK_RT2560");
	token_set_m("CONFIG_RALINK_RT2561");
	token_set_y("CONFIG_WIRELESS_TOOLS");
	token_set_y("CONFIG_HW_BUTTONS");
	token_set_y("CONFIG_USB_PRINTER");
        enable_module("CONFIG_HW_80211G_RALINK_RT2560");
        enable_module("CONFIG_HW_80211G_RALINK_RT2561");
	enable_module("CONFIG_HW_80211G_ISL_SOFTMAC");

        enable_module("MODULE_RG_DSL");
	token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ST_20190");
        token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20170");
	token_set_y("CONFIG_IXP425_ADSL_USE_SPHY");
	token_set_m("CONFIG_IXP425_ATM");
	token_set_m("CONFIG_RG_PPPOE_RELAY");
	token_set_dev_type(DEV_IF_IXP425_DSL);

	token_set_y("CONFIG_FOR_VERIZON");
	token_set_y("CONFIG_FOR_QWEST");
	token_set_y("CONFIG_RG_ACTIVE_WAN");
	token_set_y("CONFIG_RG_AUTO_WAN_DETECTION");

	/* HW specific values */
	token_set_y("CONFIG_ARCH_IXP425_MI424WR");
	token_set_y("CONFIG_ARCH_IXP425_VI414WG");
	token_set_y("CONFIG_ARCH_IXP425_RI408");
	token_set_y("CONFIG_ARCH_AMAZON_BA214WG");
	token_set_y("CONFIG_BA214WG_REV30");
	token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set_y("CONFIG_RG_ATA");
	token_set("CONFIG_RG_VOIP_NUMBER_OF_LINES", "2");
	token_set_y("CONFIG_RG_OLD_XSCALE_TOOLCHAIN");
	token_set_y("CONFIG_ARCH_IXP425_COYOTE");
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
	token_set_m("CONFIG_IXP425_ETH");
	dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	token_set_m("CONFIG_HW_SWITCH_KENDIN_KS8995M");
	token_set_m("CONFIG_HW_SWITCH_MARVELL_MV88E6083");
	token_set_dev_type(DEV_IF_KS8995M_HW_SWITCH);
	token_set_dev_type(DEV_IF_MV88E6083_HW_SWITCH);
	token_set_dev_type(DEV_IF_IXP425_ETH);
	token_set_y("CONFIG_HW_MOCA");
	token_set_m("CONFIG_ENTROPIC_CLINK");
	dev_add("clink0", DEV_IF_CLINK, DEV_IF_NET_EXT);
	dev_add("clink1", DEV_IF_CLINK, DEV_IF_NET_INT);
	token_set_m("CONFIG_ENTROPIC_EN2210");
	dev_add("clink0", DEV_IF_MOCA_PCI, DEV_IF_NET_EXT);
	dev_add("clink1", DEV_IF_MOCA_PCI, DEV_IF_NET_INT);
	token_set_m("CONFIG_VINAX");
	token_set_y("CONFIG_RG_THREADS");
	dev_add("ixp1", DEV_IF_VINAX, DEV_IF_NET_EXT);
	token_set_y("CONFIG_IKANOS_VDSL");
	dev_add("ixp1", DEV_IF_IKANOS_VDSL, DEV_IF_NET_EXT);
	token_set_y("CONFIG_HW_LEDS");	

	token_set_y("CONFIG_INFINEON_AMAZON");
	token_set_dev_type(DEV_IF_ADM6996_HW_SWITCH);
	token_set_dev_type(DEV_IF_AMAZON_ADSL);
	token_set_dev_type(DEV_IF_ETHOA);
	token_set_dev_type(DEV_IF_CLINK);
	token_set_dev_type(DEV_IF_MOCA_PCI);
	token_set_dev_type(DEV_IF_USB_RNDIS);
	token_set_dev_type(DEV_IF_BRIDGE);

	token_set_y("CONFIG_RG_DSLHOME_VOUCHERS");

	/* Needed for rgloader */
	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");
	token_set_y("CONFIG_CX8620X_COMMON_RGLOADER");
	token_set_y("CONFIG_RG_TFTP_UPGRADE");
	token_set_m("CONFIG_RG_FASTPATH_IPTV");
    }
    else if (IS_DIST("JPKG_I386"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"i386\"");
	token_set_y("CONFIG_RG_JPKG");
        token_set_y("CONFIG_RG_JPKG_BIN");
        token_set_y("CONFIG_RG_JPKG_I386");
        token_set_y("CONFIG_PCBOX");
	token_set_y("PACKAGE_OPENRG_VPN_GATEWAY");
	set_jpkg_modules();
    }
    else if (IS_DIST("JPKG_UML"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"uml\"");
	token_set_y("CONFIG_RG_JPKG");
        token_set_y("CONFIG_RG_JPKG_BIN");
        token_set_y("CONFIG_RG_JPKG_UML");
        token_set_y("CONFIG_RG_UML");
	set_jpkg_modules();
    }
    else if (IS_DIST("JPKG_ARMV5B"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"armv5b\"");
	token_set_y("CONFIG_RG_JPKG");
	token_set_y("CONFIG_RG_JPKG_BIN");
	token_set_y("CONFIG_RG_JPKG_ARMV5B");

	token_set_y("CONFIG_ARCH_IXP425_MONTEJADE");
	token_set_y("CONFIG_ARCH_IXP425_COYOTE");

	token_set("CONFIG_IXP425_FLASH_E28F640J3","m");
	token_set_y("CONFIG_HW_USB_STORAGE");
	token_set("CONFIG_RG_SURFCONTROL_PARTNER_ID", "6005");
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
	token_set_y("CONFIG_HW_USB_HOST_OHCI");
	token_set_m("CONFIG_HW_BUTTONS");
	token_set_y("CONFIG_RG_AUDIO_MGT");

	set_jpkg_modules();
	enable_module("MODULE_RG_ADVANCED_OFFICE_SERVERS");
        enable_module("MODULE_RG_FILESERVER");
        enable_module("MODULE_RG_VOIP_OSIP");
        enable_module("MODULE_RG_VOIP_RV_MGCP");
        enable_module("MODULE_RG_VOIP_RV_H323");
	enable_module("CONFIG_HW_USB_RNDIS");
        enable_module("CONFIG_HW_ENCRYPTION");
	enable_module("CONFIG_RG_LPD");
	token_set_y("CONFIG_RG_IPP");

	token_set_m("CONFIG_RG_PPPOE_RELAY");
	token_set_y("CONFIG_IPSEC_ENC_AES");
	token_set_y("CONFIG_RG_PRINT_SERVER");
	token_set_m("CONFIG_RG_USB_SLAVE");
	token_set_y("CONFIG_IXP425_CSR_USB");
	token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	token_set_y("CONFIG_WIRELESS_TOOLS");

	token_set("CONFIG_RG_VOIP_NUMBER_OF_LINES", "4");
	token_set_y("CONFIG_RG_X509");

	enable_module("CONFIG_HW_DSP");
	token_set_y("CONFIG_RG_ATA");
	token_set_y("CONFIG_IXP425_DSR");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
        enable_module("CONFIG_HW_80211G_RALINK_RT2560");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	token_set_y("CONFIG_IXP425_COMMON_RG");

	/* HW specific values */
	token_set_y("CONFIG_ARCH_IXP425_MI424WR");
	token_set_y("CONFIG_ARCH_IXP425_VI414WG");
	token_set_y("CONFIG_ARCH_IXP425_RI408");
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
	token_set_m("CONFIG_IXP425_ETH");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS1");

	token_set_dev_type(DEV_IF_IXP425_ETH);
	token_set_dev_type(DEV_IF_BRIDGE);
	token_set_dev_type(DEV_IF_USB_RNDIS);

	/* ATM Support Configuration Section */

        enable_module("MODULE_RG_DSL");
	token_set_y("CONFIG_HW_DSL_WAN");

	token_set_y("CONFIG_HW_ST_20190");
	token_set_y("CONFIG_IXP425_ADSL_USE_SPHY");

	token_set_m("CONFIG_IXP425_ATM");

	token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20170");
	
	token_set_dev_type(DEV_IF_IXP425_DSL);

	token_set_m("CONFIG_HW_SWITCH_MARVELL_MV88E6083");
	token_set_m("CONFIG_HW_SWITCH_KENDIN_KS8995M");
	token_set_dev_type(DEV_IF_KS8995M_HW_SWITCH);
	token_set_dev_type(DEV_IF_MV88E6083_HW_SWITCH);
	token_set_m("CONFIG_ENTROPIC_CLINK");
	token_set_m("CONFIG_ENTROPIC_EN2210");
	token_set_m("CONFIG_VINAX");
	token_set_y("CONFIG_RG_THREADS");
	dev_add("ixp1", DEV_IF_VINAX, DEV_IF_NET_EXT);
	token_set_y("CONFIG_IKANOS_VDSL");
	dev_add("ixp1", DEV_IF_IKANOS_VDSL, DEV_IF_NET_EXT);
	dev_add("clink0", DEV_IF_CLINK, DEV_IF_NET_EXT);
	dev_add("clink1", DEV_IF_CLINK, DEV_IF_NET_INT);
	dev_add("clink0", DEV_IF_MOCA_PCI, DEV_IF_NET_EXT);
	dev_add("clink1", DEV_IF_MOCA_PCI, DEV_IF_NET_INT);
	token_set_y("CONFIG_HW_LEDS");

	token_set_y("CONFIG_RG_DSLHOME_VOUCHERS");
	token_set("CONFIG_BOOTLDR_UBOOT_COMP", "gzip");
	
	/* RGLOADER Support Configuration Section */
	token_set_y("CONFIG_RG_RGLOADER");
	token_set_y("CONFIG_IXP425_COMMON_RGLOADER");
    }
    else if (IS_DIST("JPKG_ARMV5L"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"armv5l\"");
	token_set_y("CONFIG_RG_JPKG");
        token_set_y("CONFIG_RG_JPKG_ARMV5L");
        token_set_y("CONFIG_RG_JPKG_BIN");
	set_jpkg_modules();
	token_set_y("CONFIG_CX8620X_COMMON");

	token_set_y("PACKAGE_OPENRG_SOHO_GATEWAY");

	token_set("CONFIG_RG_SURFCONTROL_PARTNER_ID", "6002");
	token_set_y("CONFIG_RG_BOOTSTRAP");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
    	token_set_y("CONFIG_HW_USB_STORAGE");
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
 	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");

	token_set("CONFIG_CX8620X_SDRAM_SIZE", "64");

	/* Flash chips */
	token_set_m("CONFIG_CX8620X_FLASH_TE28F640C3");
	token_set_m("CONFIG_CX8620X_FLASH_TE28F160C3");

	token_set_m("CONFIG_CX8620X_SWITCH");

	token_set_y("CONFIG_CX8620X_EHCI");

	token_set("FIRM", "Conexant");

	token_set_dev_type(DEV_IF_CX8620X_SWITCH);
	token_set_dev_type(DEV_IF_BRIDGE);
    }
    else if (IS_DIST("JPKG_PPC"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"ppc\"");
	token_set_y("CONFIG_RG_JPKG");
        token_set_y("CONFIG_RG_JPKG_PPC");
        token_set_y("CONFIG_RG_JPKG_BIN");
	
	set_jpkg_modules();
	
	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_PRINTSERVER");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");

	token_set_y("CONFIG_RG_DATE");
	token_set_y("CONFIG_RG_TZ_FULL");
	token_set("CONFIG_RG_TZ_YEARS", "5");
	token_set("CONFIG_RG_SURFCONTROL_PARTNER_ID", "6005");
	token_set("CONFIG_RG_PRINT_SERVER_SPOOL", "16777216");

	/* HW Configuration Section */
	
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
	token_set_y("CONFIG_HW_USB_HOST_EHCI");
	token_set_y("CONFIG_HW_USB_HOST_OHCI");
	token_set_y("CONFIG_HW_USB_HOST_UHCI");
	token_set_y("CONFIG_HW_USB_STORAGE");
	token_set_y("CONFIG_GENERIC_ISA_DMA"); 
	
	set_big_endian(1);
	token_set_y("CONFIG_MPC8272ADS");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0" );

	token_set_y("CONFIG_FEC_ENET");
	token_set_y("CONFIG_FCC1_ENET");
	token_set_y("CONFIG_FCC2_ENET");

	token_set("CONFIG_RALINK_RT2560_TIMECSR", "0x40");
	
	token_set_dev_type(DEV_IF_MPC82XX_ETH);
	token_set_dev_type(DEV_IF_BRIDGE);
    }
    else if (IS_DIST("JPKG_MIPSEB"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"mipseb\"");
	token_set_y("CONFIG_RG_JPKG");
        token_set_y("CONFIG_RG_JPKG_MIPSEB");
        token_set_y("CONFIG_RG_JPKG_BIN");
	
	set_jpkg_modules();
	
	/* HW Configuration Section */

        token_set_y("CONFIG_HW_LEDS");
	token_set_y("CONFIG_MTD");

	token_set_y("CONFIG_ARCH_AMAZON_BA214WG");
	token_set_y("CONFIG_INFINEON_AMAZON");
	token_set_dev_type(DEV_IF_ADM6996_HW_SWITCH);
	token_set_dev_type(DEV_IF_AMAZON_ADSL);
	token_set_dev_type(DEV_IF_ETHOA);
	token_set_dev_type(DEV_IF_CLINK);
	token_set_dev_type(DEV_IF_MOCA_PCI);
	token_set_dev_type(DEV_IF_USB_RNDIS);
	token_set_dev_type(DEV_IF_BRIDGE);

	token_set_y("CONFIG_RG_DSLHOME_VOUCHERS");
	token_set_m("CONFIG_RG_FASTPATH_IPTV");
    }
    else if (IS_DIST("JPKG_XSCALE"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"xscale\"");
	token_set_y("CONFIG_RG_JPKG");
        token_set_y("CONFIG_RG_JPKG_XSCALE");
        token_set_y("CONFIG_RG_JPKG_BIN");
	set_jpkg_modules();
    }
    else if (IS_DIST("JPKG_ARM_920T_LE"))
    {
	hw = "JPKG";
        token_set("JPKG_ARCH", "\"arm-920t-le\"");
	token_set_y("CONFIG_RG_JPKG");
        token_set_y("CONFIG_RG_JPKG_ARM_920T_LE");
        token_set_y("CONFIG_RG_JPKG_BIN");
	set_jpkg_basic_modules();

	enable_module("MODULE_RG_FILESERVER");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	token_set_y("CONFIG_RG_EVENT_LOGGING");
	
	/* XXX this is a workaround until B22301 is resolved */
	token_set_y("CONFIG_RG_THREADS");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_m("CONFIG_SL2312_ETH");
 	token_set_y("CONFIG_HW_80211G_RALINK_RT2560");
	token_set_y("CONFIG_HW_USB_STORAGE");
	token_set_y("CONFIG_RG_USB");
	token_set_y("CONFIG_USB_DEVICEFS");
	token_set_m("CONFIG_USB_OHCI_SL2312");
	
	token_set_y("CONFIG_SL2312_COMMON_RG");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "8"); 

	token_set_y("CONFIG_ARCH_SL2312"); 
 	token_set_y("CONFIG_SL2312_ASIC"); 
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttySI0");
	token_set_m("CONFIG_SL2312_FLASH");
	token_set("CONFIG_SDRAM_SIZE", "64");
	
	token_set("FIRM", "Storlink");
	token_set("BOARD", "SL2312");

	token_set_dev_type(DEV_IF_BRIDGE);
	token_set_dev_type(DEV_IF_SL2312_ETH);
    }
    else if (IS_DIST("UML_LSP"))
    {
	hw = "UML";
	os = "LINUX_24";

	token_set_y("CONFIG_RG_UML");

	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_RG_PERM_STORAGE");
    }
    else if (IS_DIST("UML") || IS_DIST("UML_ATM"))
    {
	hw = "UML";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_QOS");
	enable_module("MODULE_RG_DSLHOME");

	enable_module("MODULE_RG_MAIL_FILTER");
	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	if (IS_DIST("UML_ATM"))
	{
	    enable_module("MODULE_RG_DSL");
	    token_set_y("CONFIG_HW_DSL_WAN");
	}
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_UML");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");
	token_set_y("CONFIG_RG_PROD_IMG");

	dev_add_bridge("br0", DEV_IF_NET_INT, "eth1", "eth2", NULL);
    }
    else if (IS_DIST("UML_BHR"))
    {
	hw = "UML";
    
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	token_set_y("CONFIG_RG_8021Q_IF");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");

	token_set_m("CONFIG_RG_PPPOE_RELAY"); /* PPPoE Relay */
	token_set_y("CONFIG_RG_DSLHOME");
	enable_module("MODULE_RG_QOS");

	token_set_y("CONFIG_GUI_ACTIONTEC");
	token_set_y("CONFIG_RG_FIRMWARE_RESTORE");
	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_ETH_LAN2");
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_UML");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	dev_add_bridge("br0", DEV_IF_NET_INT, "eth1", "eth2", NULL);

	/* Dist specific configuration */
	token_set_y("CONFIG_RG_PROD_IMG");

	token_set("RG_PROD_STR", "Actiontec Home Router");
	
	/* Tasklet me harder */
	token_set_y("CONFIG_TASKLET_ME_HARDER");

	/* Make readonly GUI */
	token_set_y("CONFIG_RG_WBM_READONLY_USERS_GROUPS");

	token_set_y("CONFIG_RG_DSLHOME_VOUCHERS");
    }
    else if (IS_DIST("UML_VALGRIND") || IS_DIST("UML_ATM_VALGRIND"))
    {
	hw = "UML";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_DSLHOME");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	if (IS_DIST("UML_ATM_VALGRIND"))
	{
	    enable_module("MODULE_RG_DSL");
	    token_set_y("CONFIG_HW_DSL_WAN");
	}

	token_set_y("CONFIG_HW_ETH_LAN");

	token_set_y("CONFIG_VALGRIND");
	token_set_y("CONFIG_RG_PROD_IMG");
	enable_module("MODULE_RG_QOS");

	token_set_y("CONFIG_RG_DSLHOME_VOUCHERS");
    }
    else if (IS_DIST("UML_IPPHONE_VALGRIND"))
    {
	hw = "UML";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_DSP");

	token_set_y("CONFIG_VALGRIND");

	/* VoIP */
	token_set_y("CONFIG_RG_IPPHONE");
	token_set_y("CONFIG_RG_VOIP_HW_EMULATION");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");
	token_set_y("CONFIG_RG_PROD_IMG");
    }
    else if (IS_DIST("UML_ATA") || IS_DIST("UML_ATA_OSIP") ||
	IS_DIST("UML_ATA_VALGRIND") || IS_DIST("UML_ATA_OSIP_VALGRIND"))
    {
	hw = "UML";

	token_set_y("CONFIG_RG_SMB");

	/* ALL OpenRG Available Modules - ALLMODS */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	/* For Customer distributions only:
	 * When removing IPV6 you must replace in feature_config.c the line 
	 * if(token_get("MODULE_RG_IPV6")) with if(1) */
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");

	enable_module("MODULE_RG_VOIP_OSIP");
	if (!(IS_DIST("UML_ATA_OSIP") || IS_DIST("UML_ATA_OSIP_VALGRIND")))
	{
	    enable_module("MODULE_RG_VOIP_RV_MGCP");
	    enable_module("MODULE_RG_VOIP_RV_H323");
	}

	if (IS_DIST("UML_ATA_OSIP_VALGRIND"))
	    token_set_y("CONFIG_VALGRIND");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	enable_module("CONFIG_HW_DSP");

	if (IS_DIST("UML_ATA_VALGRIND"))
	    token_set_y("CONFIG_VALGRIND");

	/* VoIP */
	token_set_y("CONFIG_RG_ATA");
	token_set_y("CONFIG_RG_VOIP_HW_EMULATION");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");
	token_set_y("CONFIG_RG_PROD_IMG");
    }
    else if (IS_DIST("MI424WR") || IS_DIST("MI424WR_VOIP") ||
	IS_DIST("MI524WR"))
    {
	char *lan_eth = "ixp1";

	if (IS_DIST("MI524WR"))
	    hw = "MI524WR";
	else
	    hw = "MI424WR";

	if (token_get("CONFIG_MI424WR_REV2A"))
	    lan_eth = "ixp0"; 
    
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5B");

	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	token_set_y("CONFIG_RG_8021Q_IF");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");

	token_set_m("CONFIG_RG_PPPOE_RELAY"); /* PPPoE Relay */
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_QOS");

	token_set_y("CONFIG_GUI_ACTIONTEC");
	token_set_y("CONFIG_FOR_VERIZON");
	token_set_y("CONFIG_RG_FIRMWARE_RESTORE");
	token_set_y("CONFIG_RG_ACTIVE_WAN");
	token_set_y("CONFIG_RG_AUTO_WAN_DETECTION");

	/* Download image to memory before flashing
	 * Only one image section in flash, enough memory */
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	if (token_get("MODULE_RG_DSL"))
	    token_set_y("CONFIG_HW_DSL_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_BUTTONS");
	token_set_y("CONFIG_HW_MOCA");
	token_set_y("CONFIG_HW_LEDS");
	if (token_get("CONFIG_RALINK_RT2561"))
	    enable_module("CONFIG_HW_80211G_RALINK_RT2561");
	else
	    enable_module("CONFIG_HW_80211G_RALINK_RT2560");

	token_set("CONFIG_RG_SSID_NAME", "openrg");
	dev_add_bridge("br0", DEV_IF_NET_INT, lan_eth, "ra0", "clink1", NULL);
	
	/* Dist specific configuration */
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_RG_PROD_IMG");

	token_set_y("CONFIG_RG_VENDOR_FACTORY_SETTINGS");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	if (IS_DIST("MI424WR_VOIP"))
	{
	    token_set_y("MODULE_RG_VOIP_OSIP");
	    token_set_y("CONFIG_HW_DSP");
	    token_set("CONFIG_RG_VOIP_NUMBER_OF_LINES", "2");
	}

	token_set("RG_PROD_STR", "Actiontec Home Router");
	
	/* Tasklet me harder */
	token_set_y("CONFIG_TASKLET_ME_HARDER");

	/* Make readonly GUI */
	token_set_y("CONFIG_RG_WBM_READONLY_USERS_GROUPS");

	token_set_y("CONFIG_RG_DSLHOME_VOUCHERS");
	token_set_y("CONFIG_ARM_24_FAST_MODULES");
    }
    else if (IS_DIST("MI424WR_LSP"))
    {
	hw = "MI424WR";

	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_RG_LSP");

	token_set_m("CONFIG_BRIDGE");
	token_set_y("CONFIG_BRIDGE_UTILS");	
	token_set_y("CONFIG_RG_NETTOOLS_ARP");
	token_set_y("CONFIG_RG_NETKIT");
	token_set_m("CONFIG_ISL_SOFTMAC");
	token_set_y("CONFIG_RG_DISABLE_LICENSE");
	
	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_MOCA");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	/* Make readonly GUI */
	token_set_y("CONFIG_RG_WBM_READONLY_USERS_GROUPS");

	token_set_y("CONFIG_ARM_24_FAST_MODULES");
    }
    else if (IS_DIST("VI414WG") || IS_DIST("VI414WG_ETH") ||
	IS_DIST("KI414WG") || IS_DIST("KI414WG_ETH"))
    {
	hw = dist;

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5B");
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	token_set_y("CONFIG_RG_8021Q_IF");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_QOS");

	token_set_m("CONFIG_RG_PPPOE_RELAY"); /* PPPoE Relay */
	enable_module("MODULE_RG_DSLHOME");

	token_set_y("CONFIG_GUI_ACTIONTEC");
	token_set_y("CONFIG_FOR_VERIZON");
	token_set_y("CONFIG_RG_FIRMWARE_RESTORE");
	token_set_y("CONFIG_RG_AUTO_WAN_DETECTION");

	/* Download image to memory before flashing
	 * Only one image section in flash, enough memory */
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");

	/* HW Configuration Section */
	if (IS_DIST("VI414WG_ETH") || IS_DIST("KI414WG_ETH"))
	    token_set_y("CONFIG_HW_ETH_WAN");
	else
	    token_set_y("CONFIG_HW_VDSL");

	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_BUTTONS");
	token_set_y("CONFIG_HW_MOCA");
	token_set_y("CONFIG_HW_LEDS");
	if (token_get("CONFIG_RALINK_RT2561"))
	    enable_module("CONFIG_HW_80211G_RALINK_RT2561");
	else
	    enable_module("CONFIG_HW_80211G_RALINK_RT2560");

	token_set("CONFIG_RG_SSID_NAME", "openrg");
	dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "ra0", "clink0", NULL);
	
	/* Dist specific configuration */
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_RG_PROD_IMG");

	token_set_y("CONFIG_RG_VENDOR_FACTORY_SETTINGS");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	token_set("RG_PROD_STR", "Actiontec Home Router");
	
	/* Tasklet me harder */
	token_set_y("CONFIG_TASKLET_ME_HARDER");

	/* Make readonly GUI */
	token_set_y("CONFIG_RG_WBM_READONLY_USERS_GROUPS");
	token_set_y("CONFIG_RG_DSLHOME_VOUCHERS");

	token_set_y("CONFIG_ARM_24_FAST_MODULES");
    }
    else if (IS_DIST("VI414WG_LSP"))
    {
	hw = "VI414WG";

	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_RG_LSP");

	token_set_m("CONFIG_BRIDGE");
	token_set_y("CONFIG_BRIDGE_UTILS");	
	token_set_y("CONFIG_RG_NETTOOLS_ARP");
	token_set_y("CONFIG_RG_NETKIT");
	token_set_m("CONFIG_ISL_SOFTMAC");
	token_set_y("CONFIG_RG_DISABLE_LICENSE");
	
	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_MOCA");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	/* Make readonly GUI */
	token_set_y("CONFIG_RG_WBM_READONLY_USERS_GROUPS");

	token_set_y("CONFIG_ARM_24_FAST_MODULES");
    }
    else if (IS_DIST("BA214WG"))
    {
	hw = "BA214WG";
    
	token_set("CONFIG_RG_JPKG_DIST", "JPKG_MIPSEB");
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	token_set_y("CONFIG_RG_8021Q_IF");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_QOS");
	//token_set_m("CONFIG_RG_PPPOE_RELAY"); /* PPPoE Relay */
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_DSL");

	token_set_y("CONFIG_GUI_ACTIONTEC");
	token_set_y("CONFIG_FOR_QWEST");
	token_set_y("CONFIG_RG_FIRMWARE_RESTORE");

	/* Download image to memory before flashing
	 * Only one image section in flash, enough memory */
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");

	token_set_y("CONFIG_ULIBC");
	token_set_y("CONFIG_DYN_LINK");

	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_LAN");
	if (token_get("MODULE_RG_DSL"))
	    token_set_y("CONFIG_HW_DSL_WAN");	
	token_set_y("CONFIG_HW_BUTTONS");
	token_set_y("CONFIG_HW_MOCA");
	token_set_y("CONFIG_HW_LEDS");
	if (token_get("CONFIG_RALINK_RT2560"))
	    enable_module("CONFIG_HW_80211G_RALINK_RT2560");
	else
	    enable_module("CONFIG_HW_80211G_RALINK_RT2561");

	token_set("CONFIG_RG_SSID_NAME", "openrg");
	dev_add_bridge("br0", DEV_IF_NET_INT, "eth1", "ra0", "clink0", NULL);
	
	/* Dist specific configuration */
	//token_set_y("CONFIG_RG_PROD_IMG");

	token_set_y("CONFIG_RG_VENDOR_FACTORY_SETTINGS");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	token_set("RG_PROD_STR", "Actiontec Home Router");
	
	/* Tasklet me harder */
	token_set_y("CONFIG_TASKLET_ME_HARDER");

	/* Make readonly GUI */
	token_set_y("CONFIG_RG_WBM_READONLY_USERS_GROUPS");
	token_set_y("CONFIG_RG_DSLHOME_VOUCHERS");
    }
    else if (IS_DIST("RI408") || IS_DIST("RI408_WIRELESS"))
    {
	hw = "RI408";

	token_set("CONFIG_RG_JPKG_DIST", "JPKG_ARMV5B");
	enable_module("MODULE_RG_FOUNDATION");
        if (IS_DIST("RI408_WIRELESS"))
	    enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	token_set_y("CONFIG_RG_8021Q_IF");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");

	token_set_m("CONFIG_RG_PPPOE_RELAY"); /* PPPoE Relay */
	enable_module("MODULE_RG_DSLHOME");
	enable_module("MODULE_RG_QOS");

	token_set_y("CONFIG_GUI_ACTIONTEC");
	token_set_y("CONFIG_FOR_VERIZON");
	token_set_y("CONFIG_RG_FIRMWARE_RESTORE");
	token_set_y("CONFIG_RG_AUTO_WAN_DETECTION");
	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_HW_BUTTONS");
	token_set_y("CONFIG_HW_LEDS");
        if (IS_DIST("RI408_WIRELESS"))
	{
	    enable_module("CONFIG_HW_80211G_RALINK_RT2560");
	    token_set("CONFIG_RG_SSID_NAME", "openrg");
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", "ra0", NULL);
	}
	else
	{
	    /* Bridge must exist in order to STP to work. */
	    dev_add_bridge("br0", DEV_IF_NET_INT, "ixp0", NULL);
	}
	
	/* Dist specific configuration */
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_RG_PROD_IMG");

	token_set_y("CONFIG_RG_VENDOR_FACTORY_SETTINGS");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	token_set("RG_PROD_STR", "Actiontec Home Router");
	
	/* Tasklet me harder */
	token_set_y("CONFIG_TASKLET_ME_HARDER");

	/* Make readonly GUI */
	token_set_y("CONFIG_RG_WBM_READONLY_USERS_GROUPS");

	token_set_y("CONFIG_RG_DSLHOME_VOUCHERS");
	token_set_y("CONFIG_ARM_24_FAST_MODULES");
    }
    else if (IS_DIST("RI408WR_LSP"))
    {
	hw = "RI408";

	token_set_y("CONFIG_LSP_DIST");
	token_set_y("CONFIG_RG_LSP");

	token_set_m("CONFIG_BRIDGE");
	token_set_y("CONFIG_BRIDGE_UTILS");	
	token_set_y("CONFIG_RG_NETTOOLS_ARP");
	token_set_y("CONFIG_RG_NETKIT");
	token_set_m("CONFIG_ISL_SOFTMAC");
	token_set_y("CONFIG_RG_DISABLE_LICENSE");
	
	/* HW Configuration Section */
	token_set_y("CONFIG_HW_ETH_WAN");
	token_set_y("CONFIG_HW_ETH_LAN");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");
    }
    else if (IS_DIST("FULL"))
    {
	token_set_y("CONFIG_RG_SMB");

	/* All OpenRG available Modules */
	enable_module("MODULE_RG_FOUNDATION");
	enable_module("MODULE_RG_ZERO_CONFIGURATION");
	enable_module("MODULE_RG_PPP");
	enable_module("MODULE_RG_FIREWALL_AND_SECURITY");
	enable_module("MODULE_RG_ADVANCED_MANAGEMENT");
	enable_module("MODULE_RG_IPSEC");
	enable_module("MODULE_RG_PPTP");
	enable_module("MODULE_RG_SNMP");
	enable_module("MODULE_RG_IPV6");
	enable_module("MODULE_RG_VLAN");
	enable_module("MODULE_RG_ADVANCED_ROUTING");
	enable_module("MODULE_RG_L2TP_CLIENT");
	enable_module("MODULE_RG_URL_FILTERING");
	enable_module("MODULE_RG_VOIP_RV_SIP");
	enable_module("MODULE_RG_VOIP_RV_MGCP");
	enable_module("MODULE_RG_VOIP_RV_H323");
        enable_module("MODULE_RG_DSL");
        enable_module("MODULE_RG_FILESERVER");
        enable_module("MODULE_RG_PRINTSERVER");
        enable_module("MODULE_RG_WLAN_AND_ADVANCED_WLAN");
        enable_module("MODULE_RG_CABLEHOME");
        enable_module("MODULE_RG_VODSL");
    }
    else
	conf_err("invalid DIST=%s\n", dist);

    token_set("CONFIG_RG_DIST", dist);
}
