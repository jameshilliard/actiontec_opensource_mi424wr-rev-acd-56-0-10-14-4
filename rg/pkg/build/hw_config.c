/****************************************************************************
 *
 * rg/pkg/build/hw_config.c
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
#include "config_opt.h"
#include "create_config.h"

void ralink_rt2560_add(void)
{
    if (token_get("MODULE_RG_WLAN_AND_ADVANCED_WLAN"))
    {
        token_set_y("CONFIG_80211G_AP_ADVANCED");
	token_set_y("CONFIG_RG_VENDOR_WLAN_SEC");
	token_set_y("CONFIG_RG_WPA_WBM");
	token_set_y("CONFIG_RG_RADIUS_WBM");
	token_set_y("CONFIG_RG_8021X_WBM");
	token_set_y("CONFIG_RG_WLAN_AUTO_CHANNEL_SELECT");
    }
    token_set_m("CONFIG_RALINK_RT2560"); 
    if (token_get("CONFIG_RG_JPKG"))
    {
	token_set_dev_type(DEV_IF_RT2560);
    }
    else
    {
	dev_add("ra0", DEV_IF_RT2560, DEV_IF_NET_INT); 
	dev_can_be_missing("ra0");
    }
}

void ralink_rt2561_add(void)
{
    if (token_get("MODULE_RG_WLAN_AND_ADVANCED_WLAN"))
    {
        token_set_y("CONFIG_80211G_AP_ADVANCED");
	token_set_y("CONFIG_RG_VENDOR_WLAN_SEC");
	token_set_y("CONFIG_RG_WPA_WBM");
	token_set_y("CONFIG_RG_RADIUS_WBM");
	token_set_y("CONFIG_RG_8021X_WBM");
	token_set_y("CONFIG_RG_WLAN_AUTO_CHANNEL_SELECT");
    }
    token_set_m("CONFIG_RALINK_RT2561"); 
    if (token_get("CONFIG_RG_JPKG"))
    {
	token_set_dev_type(DEV_IF_RT2561);
    }
    else
    {
	dev_add("ra0", DEV_IF_RT2561, DEV_IF_NET_INT); 
	dev_can_be_missing("ra0");
    }
}

void bcm43xx_add(char *wl_name)
{
    dev_add(wl_name, DEV_IF_BCM43XX, DEV_IF_NET_INT);

    if (token_get("MODULE_RG_WLAN_AND_ADVANCED_WLAN"))
    {
	token_set_y("CONFIG_RG_VENDOR_WLAN_SEC");
	token_set_y("CONFIG_RG_8021X_MD5");
	token_set_y("CONFIG_RG_8021X_TLS");
	token_set_y("CONFIG_RG_8021X_TTLS");
	token_set_y("CONFIG_RG_8021X_RADIUS");
	token_set_y("CONFIG_RG_RADIUS");
	token_set_y("CONFIG_RG_WPA_BCM");
    }
}

void isl_softmac_add(void)
{
    token_set_m("CONFIG_ISL_SOFTMAC");
    dev_add("eth0", DEV_IF_ISL_SOFTMAC, DEV_IF_NET_INT);
    token_set_y("CONFIG_RG_WLAN_AUTO_CHANNEL_SELECT");
    dev_can_be_missing("eth0");
}

void hardware_features(void)
{
    option_t *hw_tok = option_token_get(openrg_hw_options, hw);

    if (!hw_tok->value)
	conf_err("No description available for HW=%s\n", hw);

    token_set("CONFIG_RG_HW", hw);
    token_set("CONFIG_RG_HW_DESC_STR", hw_tok->value);

    if (IS_HW("ALLWELL_RTL_RTL"))
    {
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_8139TOO");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("rtl0", DEV_IF_RTL8139, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("rtl1", DEV_IF_RTL8139, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("rtl0", DEV_IF_RTL8139, DEV_IF_NET_INT);

	token_set_y("CONFIG_PCBOX");
    }

    if (IS_HW("WADB100G"))
    {
	token_set_y("CONFIG_BCM963XX_COMMON");
	token_set_y("CONFIG_BCM96348");
	token_set_y("CONFIG_BCM963XX_SERIAL");
	token_set_m("CONFIG_BCM963XX_BOARD");
	token_set_m("CONFIG_BCM963XX_MTD");
	token_set("CONFIG_BCM963XX_BOARD_ID", "96348GW-10");
	token_set("CONFIG_BCM963XX_SDRAM_SIZE", "16");

	token_set_y("CONFIG_PCI");

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_y("CONFIG_ATM");
	    token_set_m("CONFIG_BCM963XX_ADSL");
	    token_set_m("CONFIG_BCM963XX_ATM");
	    token_set_y("CONFIG_RG_ATM_QOS");
	    dev_add("bcm_atm0", DEV_IF_BCM963XX_ADSL, DEV_IF_NET_EXT);
	}
	
	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_BCM963XX_ETH");
	    dev_add("bcm0", DEV_IF_BCM963XX_ETH, DEV_IF_NET_INT);
	    dev_add("bcm1", DEV_IF_BCM963XX_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_BCM43XX"))
	{
	    token_set_y("CONFIG_NET_RADIO");
	    token_set_y("CONFIG_NET_WIRELESS");
	    bcm43xx_add("wl0");

	    if (token_get("MODULE_RG_WLAN_AND_ADVANCED_WLAN"))
	       token_set_y("CONFIG_RG_WPA_BCM");
	}

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_y("CONFIG_USB_RNDIS");
	    token_set_m("CONFIG_BCM963XX_USB");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_LEDS"))
	    token_set_y("CONFIG_RG_UIEVENTS");

	token_set("BOARD", "WADB100G");
	token_set("FIRM", "Broadcom");
    }
	
    if (IS_HW("MPC8272ADS"))
    {
	token_set_y("CONFIG_MPC8272ADS");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0" );

	token_set("FIRM", "freescale");
	token_set("BOARD", "MPC8272ADS");

	/* Ethernet drivers */
	
	if (token_get("CONFIG_HW_ETH_EEPRO1000"))
	{
	    token_set_y("CONFIG_E1000");
	    
	    dev_add("eth0", DEV_IF_EEPRO1000, DEV_IF_NET_EXT);
	    dev_add("eth1", DEV_IF_EEPRO1000, DEV_IF_NET_INT);
	}
	else
	{
	    token_set_y("CONFIG_FEC_ENET");
	    token_set_y("CONFIG_FCC1_ENET");
	    token_set_y("CONFIG_FCC2_ENET");

	    dev_add("eth0", DEV_IF_MPC82XX_ETH, DEV_IF_NET_EXT);
	    dev_add("eth1", DEV_IF_MPC82XX_ETH, DEV_IF_NET_INT);
	}
	
	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth2", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	    dev_can_be_missing("eth2");
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	{
	    ralink_rt2560_add();
	    token_set("CONFIG_RALINK_RT2560_TIMECSR", "0x40");
	}
	
	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}
    }

    if (IS_HW("ALLWELL_RTL_RTL_ISL38XX"))
    {
	token_set_m("CONFIG_8139TOO");
	if (token_get("CONFIG_RG_RGLOADER"))
	{
	    dev_add("rtl1", DEV_IF_RTL8139, DEV_IF_NET_INT);
	    dev_add("rtl0", DEV_IF_RTL8139, DEV_IF_NET_INT);
	}
	else
	{
	    dev_add("rtl0", DEV_IF_RTL8139, DEV_IF_NET_EXT);
	    dev_add("rtl1", DEV_IF_RTL8139, DEV_IF_NET_INT);
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	}
	token_set_y("CONFIG_PCBOX");
    }

    if (IS_HW("ALLWELL_RTL_EEP"))
    {
	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    token_set_m("CONFIG_8139TOO");
	    dev_add("rtl0", DEV_IF_RTL8139, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_EEPRO100");
	    dev_add("eep0", DEV_IF_EEPRO100, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ETH_LAN2"))
	{
	    token_set_m("CONFIG_8139TOO");
	    dev_add("rtl0", DEV_IF_RTL8139, DEV_IF_NET_INT);
	}
	
	token_set_y("CONFIG_PCBOX");
    }

    if (IS_HW("ALLWELL_ATMNULL_RTL"))
    {
	token_set_m("CONFIG_8139TOO");
	token_set_y("CONFIG_ATM");
	token_set_y("CONFIG_ATM_NULL");
	dev_add("rtl0", DEV_IF_RTL8139, DEV_IF_NET_INT);
	dev_add("atmnull0", DEV_IF_ATM_NULL, DEV_IF_NET_EXT);
	token_set_y("CONFIG_PCBOX");
    }

    if (IS_HW("PCBOX_EEP_EEP"))
    {
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_EEPRO100");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("eep0", DEV_IF_EEPRO100, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("eep1", DEV_IF_EEPRO100, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("eep0", DEV_IF_EEPRO100, DEV_IF_NET_INT);

	token_set_y("CONFIG_PCBOX");
    }

    if (IS_HW("CX82100"))
    {
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_CNXT_EMAC");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("cnx0", DEV_IF_CX821XX_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("cnx1", DEV_IF_CX821XX_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("cnx1", DEV_IF_CX821XX_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_USB_RNDIS"))
	    token_set_y("CONFIG_USB_RNDIS");

	token_set_y("CONFIG_ARMNOMMU");
	token_set_y("CONFIG_CX821XX_COMMON");
	token_set_y("CONFIG_BD_GOLDENGATE");
	token_set_y("CONFIG_CHIP_CX82100");
	token_set_y("CONFIG_PHY_KS8737");

	token_set("FIRM", "Conexant");
	token_set("BOARD", "CX82100");
    }

    if (IS_HW("ADM5120P"))
    {
	token_set_y("CONFIG_ADM5120_COMMON");

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("adm0", DEV_IF_ADM5120_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("adm1", DEV_IF_ADM5120_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_m("CONFIG_VINETIC");
	    token_set("CONFIG_VINETIC_LINES_PER_CHIP", "2");
	    token_set_y("CONFIG_RG_ATA");
	    token_set("CONFIG_RG_VOIP_NUMBER_OF_LINES", "2");
	}
    }

    if (IS_HW("INCAIP_LSP"))
    {
	token_set_y("CONFIG_INCAIP_COMMON");

	token_set_y("CONFIG_RG_VLAN_8021Q");
	token_set_m("CONFIG_INCAIP_SWITCH");
	token_set_m("CONFIG_INCAIP_SWITCH_API");
	token_set_m("CONFIG_INCAIP_ETHERNET");
	token_set_m("CONFIG_INCAIP_KEYPAD");
	token_set_m("CONFIG_INCAIP_DSP");
	token_set_m("CONFIG_VINETIC");
	token_set_m("CONFIG_INCAIP_IOM2");
    }

    if (IS_HW("INCAIP"))
    {
	token_set_y("CONFIG_INCAIP_COMMON");

	if (token_get("CONFIG_HW_LEDS"))
	    token_set_m("CONFIG_INCAIP_LEDMATRIX");

	if (token_get("CONFIG_HW_KEYPAD"))
	    token_set_m("CONFIG_INCAIP_KEYPAD");

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_m("CONFIG_INCAIP_DSP");
	    token_set_y("CONFIG_RG_IPPHONE");
	    token_set("CONFIG_RG_VOIP_NUMBER_OF_LINES", "1");
	}
	
	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("inca0", DEV_IF_INCAIP_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("inca0", DEV_IF_INCAIP_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ENCRYPTION"))
	    token_set_y("CONFIG_INCA_HW_ENCRYPT");
    }

    if (IS_HW("FLEXTRONICS"))
    {
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "8"); 
	token_set_y("CONFIG_INCAIP_COMMON");
	token_set_y("CONFIG_INCAIP_FLEXTRONICS");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	if (token_get("CONFIG_HW_LEDS"))
	    token_set_m("CONFIG_INCAIP_LEDMATRIX");

	if (token_get("CONFIG_HW_KEYPAD"))
	    token_set_m("CONFIG_INCAIP_KEYPAD");

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_m("CONFIG_INCAIP_DSP");
	    token_set_y("CONFIG_RG_IPPHONE");
	    token_set("CONFIG_RG_VOIP_NUMBER_OF_LINES", "1");
	}
	
	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("inca0", DEV_IF_INCAIP_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("inca0", DEV_IF_INCAIP_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ENCRYPTION"))
	    token_set_y("CONFIG_INCA_HW_ENCRYPT");
    }

    if (IS_HW("ALLTEK_VLAN"))
    {
	token_set_y("CONFIG_INCAIP_COMMON");
	token_set_y("CONFIG_INCAIP_ALLTEK");

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_m("CONFIG_VINETIC");
	    token_set("CONFIG_VINETIC_LINES_PER_CHIP", "2");
	    token_set_m("CONFIG_INCAIP_IOM2");
	    token_set_y("CONFIG_RG_ATA");
	    token_set("CONFIG_RG_VOIP_NUMBER_OF_LINES", "2");
	}
	
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_y("CONFIG_RG_VLAN_8021Q");
	    dev_add("inca0", DEV_IF_INCAIP_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("inca0.3", DEV_IF_INCAIP_VLAN, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("inca0.2", DEV_IF_INCAIP_VLAN, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ENCRYPTION"))
	    token_set_y("CONFIG_INCA_HW_ENCRYPT");
    }

    if (IS_HW("ALLTEK"))
    {
	token_set_y("CONFIG_INCAIP_COMMON");
	token_set_y("CONFIG_INCAIP_ALLTEK");
	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_m("CONFIG_VINETIC");
	    token_set("CONFIG_VINETIC_LINES_PER_CHIP", "2");
	    token_set_m("CONFIG_INCAIP_IOM2");
	    token_set_y("CONFIG_RG_ATA");
	    token_set("CONFIG_RG_VOIP_NUMBER_OF_LINES", "2");
	}
	
	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("inca0", DEV_IF_INCAIP_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ENCRYPTION"))
	    token_set_y("CONFIG_INCA_HW_ENCRYPT");
    }

    if (IS_HW("RTA770W") || IS_HW("GTWX5803"))
    {
	token_set_y("CONFIG_BCM963XX_COMMON");
	token_set_y("CONFIG_BCM96345");
	token_set_y("CONFIG_BCM963XX_SERIAL");
	token_set_m("CONFIG_BCM963XX_BOARD");
	token_set_m("CONFIG_BCM963XX_MTD");
	token_set("CONFIG_BCM963XX_SDRAM_SIZE", "16");

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_y("CONFIG_ATM");
	    token_set_m("CONFIG_BCM963XX_ADSL");
	    token_set_m("CONFIG_BCM963XX_ATM");
	    token_set_y("CONFIG_RG_ATM_QOS");
	    dev_add("bcm_atm0", DEV_IF_BCM963XX_ADSL, DEV_IF_NET_EXT);
	}
	
	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_BCM963XX_ETH");
	    dev_add("bcm0", DEV_IF_BCM963XX_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_BCM43XX"))
	{
	    token_set_y("CONFIG_NET_RADIO");
	    token_set_y("CONFIG_NET_WIRELESS");
	    bcm43xx_add("wl0");
	}

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_y("CONFIG_USB_RNDIS");
	    token_set_m("CONFIG_BCM963XX_USB");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_LEDS"))
	    token_set_y("CONFIG_RG_UIEVENTS");

	if (IS_HW("RTA770W"))
	{
	    token_set("CONFIG_BCM963XX_BOARD_ID", "RTA770W");
	    token_set("BOARD", "RTA770W");
	    token_set("FIRM", "Belkin");
	}
	else
	{
	    token_set("CONFIG_BCM963XX_BOARD_ID", "GTWX5803");
	    token_set("BOARD", "GTWX5803");
	    token_set("FIRM", "Gemtek");
	}
    }

    if (IS_HW("BCM94702"))
    {
	token_set_y("CONFIG_BCM947_COMMON");
	token_set_y("CONFIG_BCM4702");

	/* In order to make an root cramfs based dist use the following 
	 * instead of SIMPLE_RAMDISK
	 *  token_set_y("CONFIG_CRAMFS");
	 *  token_set_y("CONFIG_SIMPLE_CRAMDISK");
	 *  token_set("CONFIG_CMDLINE", 
	 *      "\"root=/dev/mtdblock2 noinitrd console=ttyS0,115200\"");
	 */

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_ET");
	    token_set_y("CONFIG_ET_47XX");
	}

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("bcm1", DEV_IF_BCM4710_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("bcm0", DEV_IF_BCM4710_ETH, DEV_IF_NET_INT);

	token_set("BOARD", "BCM94702");
	token_set("FIRM", "Broadcom");
    }

    if (IS_HW("BCM94704"))
    {
	token_set_y("CONFIG_BCM947_COMMON");
	token_set_y("CONFIG_BCM4704");

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_ET");
	    token_set_y("CONFIG_ET_47XX");
	}

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("bcm1", DEV_IF_BCM4710_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("bcm0", DEV_IF_BCM4710_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_80211G_BCM43XX"))
	{
	    token_set_y("CONFIG_NET_RADIO");
	    bcm43xx_add("eth0");
	}

	token_set("BOARD", "BCM94704");
	token_set("FIRM", "Broadcom");
    }

    if (IS_HW("BCM94712"))
    {
	/* This means (among others) copy CRAMFS to RAM, which is much
	 * safer, but PMON/CFE currently has a limit of ~3MB when uncompressing.
	 * If your image is larger than that, either reduce image size or
	 * remove CONFIG_COPY_CRAMFS_TO_RAM for this platform. */
	token_set_y("CONFIG_BCM947_COMMON");
	token_set_y("CONFIG_BCM4710");

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_ET");
	    token_set_y("CONFIG_ET_47XX");
	    token_set_y("CONFIG_RG_VLAN_8021Q");
	}

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("bcm0.1", DEV_IF_BCM4710_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("bcm0.0", DEV_IF_BCM4710_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_80211G_BCM43XX"))
	{
	    token_set_y("CONFIG_NET_RADIO");
	    bcm43xx_add("eth0");
	}

	token_set("BOARD", "BCM94712");
	token_set("FIRM", "Broadcom");
    }

    if (IS_HW("WRT54G"))
    {
	token_set_y("CONFIG_BCM947_COMMON");
	token_set_y("CONFIG_BCM947_HW_BUG_HACK");

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_ET");
	    token_set_y("CONFIG_ET_47XX");
	    token_set_y("CONFIG_RG_VLAN_8021Q");
	    token_set_y("CONFIG_VLAN_8021Q_FAST");
	    dev_add("bcm0", DEV_IF_BCM4710_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("bcm0.1", DEV_IF_VLAN, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("bcm0.2", DEV_IF_VLAN, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_80211G_BCM43XX"))
	{
	    bcm43xx_add("eth0");
	    token_set_y("CONFIG_NET_RADIO");
	}

	token_set("BOARD", "Cybertan");
	token_set("FIRM", "Cybertan");
    }
    
    if (IS_HW("CENTROID"))
    {
	set_big_endian(0);

	/* Do not change the order of the devices definition.
	 * Storlink has a bug in their ethernet driver which compells us to `up`
	 * eth0 before eth1
	 */
	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("sl0", DEV_IF_SL2312_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("sl1", DEV_IF_SL2312_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("sl1", DEV_IF_SL2312_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN") || token_get("CONFIG_HW_ETH_WAN"))
	    token_set_m("CONFIG_SL2312_ETH");

	if (token_get("CONFIG_HW_ETH_LAN2") && token_get("CONFIG_HW_ETH_WAN"))
	    conf_err("Can't define both CONFIG_HW_ETH2 and CONFIG_HW_ETH_WAN");

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	    dev_can_be_missing("eth0");
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	    ralink_rt2560_add();
	
	if (token_get("CONFIG_HW_USB_STORAGE"))
	{
	    token_set_y("CONFIG_RG_USB");
	    token_set_y("CONFIG_USB_DEVICEFS");
	    token_set_m("CONFIG_USB_OHCI_SL2312");
	}
	
	token_set_y("CONFIG_ARCH_SL2312"); 
 	token_set_y("CONFIG_SL2312_ASIC"); 
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttySI0");
	token_set_m("CONFIG_SL2312_FLASH");
	token_set("CONFIG_SDRAM_SIZE", "64");
	
	token_set("FIRM", "Storlink");
	token_set("BOARD", "SL2312");
    }

    /* IXDP425_TMT is preserved as backwards compatability for TMT */
    if (IS_HW("IXDP425") || IS_HW("IXDP425_TMT"))
    {
	/* Larger memory is available for Richfield (256MB) or
	 * Matecumbe (128MB) but use 64 for max PCI performance
	 * rates (DMA window size = 64MB) */
	token_set_y("CONFIG_ARCH_IXP425_IXDP425"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "4");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");

	token_set("FIRM", "Intel");
	token_set("BOARD", "IXDP425");
	token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2") && token_get("CONFIG_HW_ETH_WAN"))
	    conf_err("Can't define both CONFIG_HW_ETH2 and CONFIG_HW_ETH_WAN");

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    /* ADSL Chip Alcatel 20170 on board */
	    token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20150");
	    token_set_y("CONFIG_IXP425_ADSL_USE_MPHY");
	    
	    token_set_m("CONFIG_IXP425_ATM");
	    dev_add("ixp_atm0", DEV_IF_IXP425_DSL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_m("CONFIG_RG_USB_SLAVE");
	    token_set_y("CONFIG_IXP425_CSR_USB");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	}
    }

    if (IS_HW("MATECUMBE"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	/* Larger memory is available for Richfield (256MB) or
	 * Matecumbe (128MB) but use 64 for max PCI performance
	 * rates (DMA window size = 64MB) */
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "4");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set_y("CONFIG_ARCH_IXP425_MATECUMBE");
	token_set_y("CONFIG_IXP425_CSR_USB");

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_m("CONFIG_RG_USB_SLAVE");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	token_set("FIRM", "Intel");
	token_set("BOARD", "IXDP425");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
    }

    if (IS_HW("MI424WR") || IS_HW("MI524WR"))
    {
	char *lan_eth, *wan_eth;

	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "0");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "8"); 
	token_set_y("CONFIG_ARCH_IXP425_MI424WR");

	if (token_get("CONFIG_MI424WR_REV2A"))
	{
	    lan_eth = "ixp0"; 
	    wan_eth = "ixp1";
	    token_set("CONFIG_IXP425_ETH_MAC1_PHYID", "5");
	}
	else
	{
	    lan_eth = "ixp1"; 
	    wan_eth = "ixp0";
	    token_set("CONFIG_IXP425_ETH_MAC0_PHYID", "17");
	}

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add(wan_eth, DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_HW_SWITCH_KENDIN_KS8995M");
	    dev_add(lan_eth, DEV_IF_KS8995M_HW_SWITCH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add(wan_eth, DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_MOCA"))
	{
	    if (IS_HW("MI424WR"))
	    {
		token_set_m("CONFIG_ENTROPIC_CLINK");
		dev_add("clink0", DEV_IF_CLINK, DEV_IF_NET_EXT);
		dev_add("clink1", DEV_IF_CLINK, DEV_IF_NET_INT);
	    }
	    else
	    {
		token_set_m("CONFIG_ENTROPIC_CLINK");
		token_set_m("CONFIG_ENTROPIC_EN2210");
		dev_add("clink0", DEV_IF_MOCA_PCI, DEV_IF_NET_EXT);
		dev_add("clink1", DEV_IF_MOCA_PCI, DEV_IF_NET_INT);
	    }
	    dev_can_be_missing("clink1");
	    dev_can_be_missing("clink0");
	}
	
	if (token_get("CONFIG_HW_80211G_ISL_SOFTMAC"))
	    isl_softmac_add();

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	{
	    ralink_rt2560_add();
	    token_set_y("CONFIG_WIRELESS_TOOLS");
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	{
	    ralink_rt2561_add();	
	    token_set_y("CONFIG_WIRELESS_TOOLS");
	}

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_y("CONFIG_RG_ATA");
	    token_set_y("CONFIG_IXP425_DSR");
	}

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_m("CONFIG_IXP425_ATM");
	    dev_add("ixp_atm0", DEV_IF_IXP425_DSL, DEV_IF_NET_EXT);
	}
	
	token_set("FIRM", "Actiontec");
	token_set("BOARD", "MI424WR");

	/* Flash chip */
	token_set_m("CONFIG_IXP425_FLASH_E28F640J3");
    }

    if (IS_HW("VI414WG") || IS_HW("VI414WG_ETH") || IS_HW("KI414WG") || 
	IS_HW("KI414WG_ETH"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "0");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "8"); 
	token_set_y("CONFIG_ARCH_IXP425_VI414WG");
   
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    token_set("CONFIG_IXP425_ETH_MAC1_PHYID", "17");
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_HW_SWITCH_KENDIN_KS8995M");
	    dev_add("ixp0", DEV_IF_KS8995M_HW_SWITCH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_MOCA"))
	{
	    token_set_m("CONFIG_ENTROPIC_CLINK");
	    dev_add("clink0", DEV_IF_CLINK, DEV_IF_NET_INT);
	    dev_can_be_missing("clink0");
	}
	
	if (token_get("CONFIG_HW_VDSL"))
	{
	    token_set_y("CONFIG_RG_THREADS");

	    if (IS_HW("KI414WG"))
	    {
		token_set_y("CONFIG_IKANOS_VDSL");
		dev_add("ixp1", DEV_IF_IKANOS_VDSL, DEV_IF_NET_EXT);
	    }
	    else
	    {
		token_set_m("CONFIG_VINAX");
		dev_add("ixp1", DEV_IF_VINAX, DEV_IF_NET_EXT);
	    }
	}

	if (token_get("CONFIG_HW_80211G_ISL_SOFTMAC"))
	    isl_softmac_add();

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	{
	    ralink_rt2560_add();
	    token_set_y("CONFIG_WIRELESS_TOOLS");
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	{
	    ralink_rt2561_add();	
	    token_set_y("CONFIG_WIRELESS_TOOLS");
	}

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_y("CONFIG_RG_ATA");
	    token_set_y("CONFIG_IXP425_DSR");
	}

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_m("CONFIG_IXP425_ATM");
	    dev_add("ixp_atm0", DEV_IF_IXP425_DSL, DEV_IF_NET_EXT);
	}
	
	token_set("FIRM", "Actiontec");
	token_set("BOARD", hw);

	/* Flash chip */
	if (IS_HW("KI414WG") || IS_HW("KI414WG_ETH"))
	    token_set_m("CONFIG_IXP425_FLASH_E28F128J3");
	else
	    token_set_m("CONFIG_IXP425_FLASH_E28F640J3");
    }

    if (IS_HW("BA214WG"))
    {
	token_set_y("CONFIG_INFINEON_AMAZON");
	token_set_y("CONFIG_ARCH_AMAZON_BA214WG");
	token_set_y("CONFIG_BA214WG_REV30");
	
	/* Ethernet */
	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("eth1", DEV_IF_ADM6996_HW_SWITCH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    dev_add("atm0", DEV_IF_AMAZON_ADSL, DEV_IF_NET_EXT);

	    if (token_get("CONFIG_FOR_QWEST"))
	    {
		dev_add("ethoa0", DEV_IF_ETHOA, DEV_IF_NET_EXT);
		dev_set_dependency("ethoa0", "atm0");

		dev_add("ethoa1", DEV_IF_ETHOA, DEV_IF_NET_INT);
		dev_set_dependency("ethoa1", "atm0");

		token_set_m("CONFIG_RG_FASTPATH_IPTV");
	    }
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	{
	    ralink_rt2560_add();
	    token_set_y("CONFIG_WIRELESS_TOOLS");
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	{
	    ralink_rt2561_add();	
	    token_set_y("CONFIG_WIRELESS_TOOLS");
	}

	if (token_get("CONFIG_HW_MOCA"))
	{
	    if (token_get("CONFIG_ENTROPIC_CLINK"))
		dev_add("clink0", DEV_IF_CLINK, DEV_IF_NET_INT);
	    else
	    {
		token_set_m("CONFIG_ENTROPIC_EN2210");
		dev_add("clink0", DEV_IF_MOCA_PCI, DEV_IF_NET_INT);
	    }
	    dev_can_be_missing("clink0");
	}

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

    	/* Flash/MTD */
	token_set_y("CONFIG_MTD");
	token_set_y("CONFIG_MTD_CFI_AMDSTD");
	token_set_y("CONFIG_MTD_CFI_ADV_OPTIONS");
	token_set_y("CONFIG_MTD_CFI_NOSWAP");
	token_set_y("CONFIG_MTD_CFI_GEOMETRY");
	token_set_y("CONFIG_MTD_CFI_B2");
	token_set_y("CONFIG_MTD_CFI_I1");

	/* SDRAM size */
	if (!token_get("CONFIG_SDRAM_SIZE"))
	    token_set("CONFIG_SDRAM_SIZE", "32");
    }

    if (IS_HW("RI408"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "0");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "8"); 
	token_set_y("CONFIG_ARCH_IXP425_RI408");
  
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);
	    token_set("CONFIG_IXP425_ETH_MAC1_PHYID", "31");
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_HW_SWITCH_MARVELL_MV88E6083");
	    dev_add("ixp0", DEV_IF_MV88E6083_HW_SWITCH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ETH_LAN2"))
	{
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);
	    token_set("CONFIG_IXP425_ETH_MAC1_PHYID", "31");
	}

	if (token_get("CONFIG_HW_80211G_ISL_SOFTMAC"))
	    isl_softmac_add();

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	{
	    ralink_rt2560_add();
            token_set_y("CONFIG_WIRELESS_TOOLS");
	}

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_y("CONFIG_RG_ATA");
	    token_set_y("CONFIG_IXP425_DSR");
	}

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

	token_set("FIRM", "Actiontec");
	token_set("BOARD", "RI408");

	/* Flash chip */
	token_set_m("CONFIG_IXP425_FLASH_E28F640J3");
    }

    if (IS_HW("KINGSCANYON"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "4");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set_y("CONFIG_IXP425_CSR_USB");
	token_set_y("CONFIG_ARCH_IXP425_KINGSCANYON");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_m("CONFIG_RG_USB_SLAVE");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	}

	token_set("FIRM", "Interface_Masters");
	token_set("BOARD", "KINGSCANYON");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
    }

    if (IS_HW("ROCKAWAYBEACH"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS1");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set_y("CONFIG_ARCH_IXP425_ROCKAWAYBEACH");
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	token_set("CONFIG_FILESERVER_KERNEL_CONFIG", "USB");

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_y("CONFIG_IXP425_CSR_USB");
	    token_set_m("CONFIG_RG_USB_SLAVE");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	}

	token_set("FIRM", "Intel");
	token_set("BOARD", "ROCKAWAYBEACH");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
    }
    
    if (IS_HW("WAV54G"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_WAV54G"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20170");
	token_set_y("CONFIG_IXP425_ADSL_USE_SPHY");

	/* Add VLAN support so Cybertan can add HW DMZ */
	token_set_y("CONFIG_RG_VLAN_8021Q");

	if (token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_m("CONFIG_IXP425_ATM");
	    dev_add("ixp_atm0", DEV_IF_IXP425_DSL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	    dev_can_be_missing("eth0");
	}
	
	token_set("FIRM", "Cybertan");
	token_set("BOARD", "WAV54G");

	/* Flash CHIP */
	token_set("CONFIG_IXP425_FLASH_E28F640J3", "m");

	/* Download image to memory before flashing
	 * Only one image section in flash, enough memory */
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");
    }

    if (IS_HW("NAPA"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_NAPA"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "4");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "0");

	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_y("CONFIG_IXP425_CSR_USB");
	    token_set_m("CONFIG_RG_USB_SLAVE");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	token_set("FIRM", "Sohoware");
	token_set("BOARD", "NAPA");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");

	token_set_y("CONFIG_RG_UIEVENTS");
	token_set_m("CONFIG_RG_KLEDS");
    }
	
    /* COYOTE_WIRELESS and COYOTE_ATM_WIRELESS are preserved for backwards
     * compatability in BKTEL and DIALFACE customers 
     */
    if (IS_HW("COYOTE") || IS_HW("COYOTE_WIRELESS")|| 
	IS_HW("COYOTE_ATM_WIRELESS"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_COYOTE"); 
	if (!token_get("CONFIG_RG_FLASH_LAYOUT_SIZE"))
	    token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16"); 
	if (!token_get("CONFIG_IXP425_SDRAM_SIZE"))
	    token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS1");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");

	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

	if (token_get("CONFIG_HW_HSS_WAN"))
	    token_set_y("CONFIG_RG_HSS");

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    /* ADSL Chip Alcatel 20170 on board */
	    token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20170");
	    token_set_y("CONFIG_IXP425_ADSL_USE_SPHY");
	    
	    token_set_m("CONFIG_IXP425_ATM");
	    dev_add("ixp_atm0", DEV_IF_IXP425_DSL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ETH_LAN2") && token_get("CONFIG_HW_ETH_WAN"))
	    conf_err("Can't define both CONFIG_HW_ETH2 and CONFIG_HW_ETH_WAN");

	if (token_get("CONFIG_HW_ETH_LAN2"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ETH_EEPRO100_LAN"))
	{
	    token_set_m("CONFIG_EEPRO100");
	    dev_add("eep0", DEV_IF_EEPRO100, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_m("CONFIG_RG_USB_SLAVE");
	    token_set_y("CONFIG_IXP425_CSR_USB");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	    dev_can_be_missing("eth0");
	}

	if (token_get("CONFIG_HW_80211G_ISL_SOFTMAC"))
	    isl_softmac_add();

	if (token_get("CONFIG_HW_80211B_PRISM2"))
	{
	    token_set_m("CONFIG_PRISM2_PCI");
	    dev_add("wlan0", DEV_IF_PRISM2, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	    ralink_rt2560_add();

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_y("CONFIG_RG_ATA");
	    token_set_y("CONFIG_IXP425_DSR");
	    token_set("CONFIG_RG_VOIP_NUMBER_OF_LINES", "2");
	}
	
	if (token_get("CONFIG_HW_EEPROM"))
	    token_set("CONFIG_PCF8594C2", "m");

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}
	
	if (token_get("CONFIG_IXP425_CSR_FULL"))
	{
	    /* ADSL Chip Alcatel 20170 on board */
	    token_set_y("CONFIG_ADSL_CHIP_ALCATEL_20170");
	    token_set_y("CONFIG_IXP425_ADSL_USE_SPHY");
	}

	token_set("FIRM", "Intel");
	token_set("BOARD", "COYOTE");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");

	if (token_get("CONFIG_HW_IDE"))
	{
	    /* Custom IDE device on expansion BUS */
	    token_set_y("CONFIG_IDE");
	    token_set_y("CONFIG_BLK_DEV_IDE");
	    token_set_y("CONFIG_BLK_DEV_IDEDISK");
	    token_set_y("CONFIG_IDEDISK_MULTI_MODE");
	    token_set_y("CONFIG_RG_IDE_NON_BLOCKING");
	}
    }

    if (IS_HW("MONTEJADE"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_MONTEJADE"); 
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "8"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "0");

	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_y("CONFIG_HW_ST_20190");
	    token_set_y("CONFIG_IXP425_ADSL_USE_SPHY");
	    
	    token_set_m("CONFIG_IXP425_ATM");
	    dev_add("ixp_atm0", DEV_IF_IXP425_DSL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ETH_LAN2") && token_get("CONFIG_HW_ETH_WAN"))
	    conf_err("Can't define both CONFIG_HW_ETH2 and CONFIG_HW_ETH_WAN");

	if (token_get("CONFIG_HW_ETH_LAN2"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_USB_RNDIS"))
	{
	    token_set_m("CONFIG_RG_USB_SLAVE");
	    token_set_y("CONFIG_IXP425_CSR_USB");
	    dev_add("usb0", DEV_IF_USB_RNDIS, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	    dev_can_be_missing("eth0");
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	    ralink_rt2560_add();

	if (token_get("CONFIG_HW_80211N_AIRGO_AGN100"))
	{
	    token_set_y("CONFIG_80211G_AP_ADVANCED");
	    token_set_y("CONFIG_RG_VENDOR_WLAN_SEC");
	    token_set_y("CONFIG_RG_WPA_WBM");
	    token_set_y("CONFIG_RG_RADIUS_WBM");
	    token_set_y("CONFIG_RG_8021X_WBM");
	    token_set_m("CONFIG_AGN100");
	    token_set_y("CONFIG_NETFILTER");
	    dev_add("wlan0", DEV_IF_AGN100, DEV_IF_NET_INT);
	    dev_can_be_missing("wlan0");
	}

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_y("CONFIG_RG_ATA");
	    token_set_y("CONFIG_IXP425_DSR");
	    token_set("CONFIG_RG_VOIP_NUMBER_OF_LINES", "4");
	}

	token_set("FIRM", "Intel");
	token_set("BOARD", "MONTEJADE");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F640J3","m");
    }
 
    if (IS_HW("BRUCE"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_BRUCE");
	if (!token_get("CONFIG_RG_FLASH_LAYOUT_SIZE"))
	    token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "16");
	token_set("CONFIG_IXP425_SDRAM_SIZE", "128");
	token_set("CONFIG_SDRAM_SIZE", "128");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");

	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "0");

	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ETH_LAN2") && token_get("CONFIG_HW_ETH_WAN"))
	{
	    fprintf(stderr, "Can't define CONFIG_HW_ETH2 and CONFIG_HW_ETH_WAN "
		"together\n");
	    exit(1);
	}

	if (token_get("CONFIG_HW_ETH_LAN2"))
	{
	    token_set_m("CONFIG_IXP425_ETH");
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	    dev_can_be_missing("eth0");
	}

	if (token_get("CONFIG_HW_EEPROM"))
	    token_set("CONFIG_PCF8594C2", "m");

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}
	
	token_set("FIRM", "Jabil");
	token_set("BOARD", "Bruce");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");

	if (token_get("CONFIG_HW_IDE"))
	{
	    /* SiI680ACL144 and SiI3512CT128 on PCI BUS */
	    token_set_y("CONFIG_IDE");
	    token_set_y("CONFIG_BLK_DEV_IDE");
	    token_set_y("CONFIG_BLK_DEV_IDEDISK");
	    token_set_y("CONFIG_BLK_DEV_IDECD");
	    token_set_y("CONFIG_BLK_DEV_IDEPCI");
	    token_set_y("CONFIG_BLK_DEV_IDEDMA");
	    token_set_y("CONFIG_BLK_DEV_IDEDMA_PCI");
	    token_set_y("CONFIG_BLK_DEV_OFFBOARD");
	    token_set_y("CONFIG_BLK_DEV_SIIMAGE");
	}

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

	/* I2C Bus */
	if (token_get("CONFIG_HW_I2C"))
	{
	    token_set_y("CONFIG_I2C");
	    token_set_y("CONFIG_I2C_ALGOBIT");
	    token_set_y("CONFIG_I2C_IXP425");
	}

	/* HW clock */
	if (token_get("CONFIG_HW_CLOCK"))
	    token_set_m("CONFIG_I2C_DS1374");

	/* Env. monitor clock */
	if (token_get("CONFIG_HW_ENV_MONITOR"))
	    token_set_m("CONFIG_I2C_LM81");

	/* CPLD chip module and API */
	token_set_m("CONFIG_ARCH_IXP425_BRUCE_CPLD");
	
    }

    if (IS_HW("HG21"))
    {
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_HG21"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set("CONFIG_IXDP425_KGDB_UART", "0");
	
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}
	
	token_set("FIRM", "Welltech");
	token_set("BOARD", "HG21");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F640J3","m");
	token_set("CONFIG_IXP425_FLASH_USER_PART", "0x00100000");

	/* Download image to memory before flashing
	 * Only one image section in flash, enough memory */
	token_set_y("CONFIG_RG_RMT_UPGRADE_IMG_IN_MEM");
    }
    
    if (IS_HW("BAMBOO"))
    {
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}
	
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_BAMBOO"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "4");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS1");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");
	token_set_m("CONFIG_AT93CXX");
	token_set_m("CONFIG_ADM6996");
	token_set_y("CONFIG_RG_VLAN_8021Q");

	/* CSR HSS support */
	token_set_y("CONFIG_IXP425_CSR_HSS");
	token_set("CONFIG_IXP425_CODELETS", "m");
	token_set("CONFIG_IXP425_CODELET_HSS", "m");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");

	token_set("FIRM", "Planex");
	token_set("BOARD", "BAMBOO");

	token_set("RG_GLIBC_VERSION", "2.2.3");
    }
	
    if (IS_HW("USR8200"))
    {
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_CLOCK"))
	    token_set("CONFIG_JEEVES_RTC7301", "m");

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F128J3", "m");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS1");
	token_set("FIRM", "USR");
	token_set("BOARD", "USR8200");
	token_set_y("CONFIG_ARCH_IXP425_JEEVES");
	token_set("CONFIG_IXP425_SDRAM_SIZE", "64");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
    }
    
    if (IS_HW("GTWX5715"))
    {
	if (token_get("CONFIG_HW_ETH_WAN") || token_get("CONFIG_HW_ETH_LAN"))
	    token_set_m("CONFIG_IXP425_ETH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("ixp0", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("ixp1", DEV_IF_IXP425_ETH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth0", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	}

	if (token_get("CONFIG_HW_ENCRYPTION"))
	{
	    token_set_y("CONFIG_IPSEC_USE_IXP4XX_CRYPTO");
	    token_set_y("CONFIG_IPSEC_ENC_AES");
	}
	
	token_set_y("CONFIG_GEMTEK_COMMON");
	token_set_y("CONFIG_GEMTEK_WX5715");
	token_set_y("CONFIG_IXP425_COMMON_RG");
	token_set_y("CONFIG_ARCH_IXP425_GTWX5715"); 
	token_set("CONFIG_IXP425_SDRAM_SIZE", "32");
	token_set("CONFIG_IXP425_NUMBER_OF_MEM_CHIPS", "2");
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS1");
	token_set("CONFIG_IXDP425_KGDB_UART", "1");

	/* Flash chip */
	token_set("CONFIG_IXP425_FLASH_E28F640J3", "m");

	token_set("FIRM", "Gemtek");
	token_set("BOARD", "GTWX5715");
    }

    if (IS_HW("TI_404"))
    {
	if (token_get("CONFIG_HW_CABLE_WAN"))
	{
	    dev_add("cbl0", DEV_IF_TI404_CBL, DEV_IF_NET_EXT);
	    dev_set_dependency("cbl0", "cable0");
	    dev_add("cable0", DEV_IF_DOCSIS, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("lan0", DEV_IF_TI404_LAN, DEV_IF_NET_INT);

	set_big_endian(1);
	token_set_y("CONFIG_TI_404_MIPS");
	token_set_y("CONFIG_TI_404_COMMON");
	token_set("CONFIG_ARCH_MACHINE", "ti404");
	token_set("RAM_HIGH_ADRS", "0x94F00000");
	token_set("RAM_LOW_ADRS", "0x94001000");
    }

    if (IS_HW("AR531X_G") || IS_HW("WRT108G") || IS_HW("AR531X_WRT55AG") ||
	IS_HW("AR531X_AG"))
    {
	char *size;
	int is_ag_board = IS_HW("AR531X_WRT55AG") || IS_HW("AR531X_AG");

	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    dev_add(is_ag_board ? "ae1" : "ae3", DEV_IF_AR531X_ETH,
		DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    dev_add(is_ag_board ? "ae0" : "ae2", DEV_IF_AR531X_ETH,
		DEV_IF_NET_INT);
	}

	if (IS_HW("AR531X_WRT55AG"))
	    token_set_y("CONFIG_PHY_KS8995M");
	else
	    token_set_y("CONFIG_PHY_MARVEL");

	if (token_get("CONFIG_HW_80211G_AR531X"))
	{
	    token_set_y("CONFIG_RG_VENDOR_WLAN_SEC");
	    token_set_y("CONFIG_RG_WPA_WBM");
	    token_set_y("CONFIG_RG_RADIUS_WBM");
	    token_set_y("CONFIG_RG_8021X_WBM");
	    token_set_y("CONFIG_80211G_AP_ADVANCED");
	    token_set_y("CONFIG_RG_WLAN_AUTO_CHANNEL_SELECT");
	    dev_add("vp256", DEV_IF_AR531X_WLAN_G, DEV_IF_NET_INT);
	}
	if (token_get("CONFIG_HW_80211A_AR531X"))
	    dev_add("vp0", DEV_IF_AR531X_WLAN_A, DEV_IF_NET_INT);

	token_set("ARCH", "mips");
	set_big_endian(1);
	token_set_y("CONFIG_ATHEROS_AR531X_MIPS");
	token_set("RAM_HIGH_ADRS", "0x80680000");
	token_set("RAM_LOW_ADRS", "0x80010000");
	token_set_y("CONFIG_VX_TFFS");
	token_set_y("CONFIG_RG_VX_DEFERRED_TX");

	if (IS_HW("WRT108G"))
	{
	    token_set_y("CONFIG_ARCH_WRT108G");
	    token_set_y("CONFIG_RG_WLAN_AUTO_CHANNEL_SELECT");
	}
	else
	    token_set_y("CONFIG_ARCH_AR531X");

	if ((size = token_get_str("CONFIG_SDRAM_SIZE")) && atoi(size) <= 8)
	    token_set_y("CONFIG_SMALL_SDRAM");
    }

    if (IS_HW("CX8620XR") || IS_HW("CX8620XD"))
    {
	if (IS_HW("CX8620XR"))
	{
	    token_set("CONFIG_CX8620X_SDRAM_SIZE", "8");
	    token_set_y("CONFIG_RG_BOOTSTRAP");
	    
	    /* Flash chip */
	    token_set_m("CONFIG_CX8620X_FLASH_TE28F160C3");
	    token_set("BOARD", "CX8620XR");
	}
	else
	{
	    token_set("CONFIG_CX8620X_SDRAM_SIZE", "64");
	
	    if (token_get("CONFIG_LSP_DIST"))
		token_set_y("CONFIG_RG_BOOTSTRAP");

	    /* Flash chip */
	    if (!token_get("CONFIG_CX8620X_FLASH_TE28F320C3"))
		token_set_m("CONFIG_CX8620X_FLASH_TE28F640C3");
	    token_set("BOARD", "CX8620XD");
	}
	    
	token_set_m("CONFIG_CX8620X_SWITCH");

	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("cnx1", DEV_IF_CX8620X_SWITCH, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("cnx0", DEV_IF_CX8620X_SWITCH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("cnx1", DEV_IF_CX8620X_SWITCH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_USB_HOST_EHCI"))
	    token_set_y("CONFIG_CX8620X_EHCI");
	
	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}

	if (token_get("CONFIG_HW_80211G_ISL_SOFTMAC"))
	    isl_softmac_add();
	
	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	    ralink_rt2560_add();
	
	token_set("FIRM", "Conexant");
    }

    if (IS_HW("TI_TNETWA100"))
    {
	set_big_endian(1);
	token_set_y("CONFIG_TI_404_MIPS");
	token_set_y("CONFIG_TI_404_COMMON");
	token_set("RAM_HIGH_ADRS", "0x94F00000");
	token_set("RAM_LOW_ADRS", "0x94001000");
    }

    if (IS_HW("CENTAUR"))
    {
	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "4"); 
	token_set("CONFIG_SDRAM_SIZE", "32");	
	token_set("CONFIG_RG_CONSOLE_DEVICE", "ttyS0");
	token_set_y("CONFIG_RG_DYN_FLASH_LAYOUT");

	if (token_get("CONFIG_HW_ETH_WAN"))
	{
	    token_set_y("CONFIG_KS8695");
	    dev_add("eth0", DEV_IF_KS8695_ETH, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_ETH_LAN"))
	{
	    token_set_y("CONFIG_KS8695");
	    dev_add("eth1", DEV_IF_KS8695_ETH, DEV_IF_NET_INT);
	}
	
	if (token_get("CONFIG_HW_80211G_ISL38XX"))
	{
	    token_set_m("CONFIG_ISL38XX");
	    dev_add("eth2", DEV_IF_ISL38XX, DEV_IF_NET_INT);
	    dev_can_be_missing("eth2");
	}

	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	    ralink_rt2560_add();

	if (token_get("CONFIG_HW_DSP"))
	{
	    token_set_y("CONFIG_ZSP400");
	    token_set_y("CONFIG_RG_ATA");
	    token_set("CONFIG_RG_VOIP_NUMBER_OF_LINES", "4");
	}

	token_set("FIRM", "Micrel");
	token_set("BOARD", "CENTAUR");

	/* Flash chip XXX Should be module */
	token_set_y("CONFIG_KS8695_FLASH_AM29LV033C");
    }

    if (IS_HW("I386_BOCHS"))	
    {
	set_big_endian(0);
	token_set_y("CONFIG_I386_BOCHS");
	token_set("CONFIG_ARCH_MACHINE", "i386");
	if (token_get("CONFIG_RG_OS_VXWORKS"))
	{
	    dev_add("ene0", DEV_IF_NE2K_VX, DEV_IF_NET_INT);
	    dev_add("ene1", DEV_IF_NE2K_VX, DEV_IF_NET_INT);
	    dev_add("ene2", DEV_IF_NE2K_VX, DEV_IF_NET_EXT);
	    token_set_y("CONFIG_VX_KNET_SYMLINK");
	    token_set("RAM_HIGH_ADRS", "0x00008000");
	    token_set("RAM_LOW_ADRS", "0x00108000");
	}
	else
	{
	    token_set_m("CONFIG_NE2000");
	    dev_add("ne0", DEV_IF_NE2000, DEV_IF_NET_INT);
	    dev_add("ne1", DEV_IF_NE2000, DEV_IF_NET_EXT);
	    token_set_y("CONFIG_PCBOX");
	}
    }

    if (IS_HW("UML"))
    {
	token_set("ARCH", "um");

	token_set("CONFIG_RG_FLASH_LAYOUT_SIZE", "64"); 
	if (token_get("CONFIG_HW_ETH_WAN"))
	    dev_add("eth0", DEV_IF_UML, DEV_IF_NET_EXT);

	if (token_get("CONFIG_HW_DSL_WAN"))
	{
	    token_set_y("CONFIG_ATM_NULL");
	    dev_add("atmnull0", DEV_IF_ATM_NULL, DEV_IF_NET_EXT);
	}

	if (token_get("CONFIG_HW_HSS_WAN"))
	    token_set_y("CONFIG_RG_HSS");

	if (token_get("CONFIG_HW_ETH_LAN"))
	    dev_add("eth1", DEV_IF_UML_HW_SWITCH, DEV_IF_NET_INT);

	if (token_get("CONFIG_HW_ETH_LAN2"))
	    dev_add("eth2", DEV_IF_UML, DEV_IF_NET_INT);

	token_set_y("CONFIG_RG_UML");

	token_set_y("CONFIG_DEBUGSYM"); /* UML is always for debug ;-) */
	
	token_set("CONFIG_RG_CONSOLE_DEVICE", "console");

	token_set_y("CONFIG_RAMFS");
	token_set_y("CONFIG_SIMPLE_RAMDISK");
	token_set("CONFIG_RAMDISK_SIZE", "65536");
	token_set("CONFIG_KERNEL_STACK_ORDER", "2");
	token_set_y("CONFIG_MODE_TT");
	token_set_y("CONFIG_MODE_SKAS");
	token_set("CONFIG_NEST_LEVEL", "0");
	token_set("CONFIG_CON_ZERO_CHAN", "fd:0,fd:1");
	token_set("CONFIG_CON_CHAN", "xterm");
	token_set("CONFIG_SSL_CHAN", "pty");
	token_set("CONFIG_KERNEL_HALF_GIGS", "1");
	token_set_y("CONFIG_PT_PROXY");
	token_set_y("CONFIG_STDIO_CONSOLE");
	token_set_y("CONFIG_SSL");
	token_set_y("CONFIG_FD_CHAN");
	token_set_y("CONFIG_NULL_CHAN");
	token_set_y("CONFIG_PORT_CHAN");
	token_set_y("CONFIG_PTY_CHAN");
	token_set_y("CONFIG_TTY_CHAN");
	token_set_y("CONFIG_XTERM_CHAN");
	token_set_y("CONFIG_BLK_DEV_UBD");
	token_set_y("CONFIG_BLK_DEV_NBD");
	token_set("CONFIG_BLK_DEV_RAM_SIZE", "8192");
	token_set_y("CONFIG_RG_INITFS_RAMDISK");
	token_set_y("CONFIG_UML_NET");
	token_set_y("CONFIG_UML_NET_ETHERTAP");
	token_set_y("CONFIG_UML_NET_TUNTAP");
	token_set_y("CONFIG_UML_NET_SLIP");
	token_set_y("CONFIG_UML_NET_SLIRP");
	token_set_y("CONFIG_UML_NET_DAEMON");
	token_set_y("CONFIG_UML_NET_MCAST");
	token_set_y("CONFIG_DUMMY");
	token_set_y("CONFIG_TUN");

	token_set("CONFIG_UML_RAM_SIZE",
	    token_get("CONFIG_VALGRIND") ? "128M" : "32M");

	token_set_y("CONFIG_USERMODE");
	token_set_y("CONFIG_UID16");
	token_set_y("CONFIG_EXPERIMENTAL");
	token_set_y("CONFIG_BSD_PROCESS_ACCT");
	token_set_y("CONFIG_HOSTFS");
	token_set_y("CONFIG_HPPFS");
	token_set_y("CONFIG_MCONSOLE");
	token_set_y("CONFIG_MAGIC_SYSRQ");
	token_set_y("CONFIG_PROC_MM");

	token_set_y("CONFIG_PACKET_MMAP");
	token_set_y("CONFIG_QUOTA");
	token_set_y("CONFIG_AUTOFS_FS");
	token_set_y("CONFIG_AUTOFS4_FS");
	token_set_y("CONFIG_REISERFS_FS");

	token_set_y("CONFIG_MTD_BLKMTD");
	token_set_y("CONFIG_ZLIB_INFLATE");
	token_set_y("CONFIG_ZLIB_DEFLATE");

	token_set_y("CONFIG_PT_PROXY");
	token_set_y("CONFIG_RG_THREADS");
	token_set("CONFIG_RG_KERNEL_COMP_METHOD", "gzip");
	token_set("CONFIG_RG_VOIP_NUMBER_OF_LINES", "24");
    }
    
    if (IS_HW("JPKG"))
    {
	if (token_get("CONFIG_HW_80211G_RALINK_RT2560"))
	    ralink_rt2560_add();
	if (token_get("CONFIG_HW_80211G_RALINK_RT2561"))
	    ralink_rt2561_add();
	if (token_get("CONFIG_HW_80211G_ISL_SOFTMAC"))
	    isl_softmac_add();

	token_set_y("CONFIG_RG_UIEVENTS");

	if (token_get("CONFIG_HW_LEDS"))
	{
	    token_set_y("CONFIG_RG_UIEVENTS");
	    token_set_m("CONFIG_RG_KLEDS");
	}
    }
    
    if (token_get("MODULE_RG_IPV6"))
	dev_add("sit0", DEV_IF_IPV6_OVER_IPV4_TUN, DEV_IF_NET_INT);

}

