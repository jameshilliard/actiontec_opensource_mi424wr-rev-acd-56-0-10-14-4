/****************************************************************************
 *
 * rg/pkg/include/rg_def.h
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

#ifndef _RG_DEF_H_
#define _RG_DEF_H_

#if USE_BASIC_HOST_CONFIG
#include <host_config.h>
#else
#include <rg_config.h>
#endif

#define OS_INCLUDE_STRING
#include <os_includes.h>

#include <util/util.h>
#include <rg_types.h>

#define UNUSED_SYM __attribute__((__unused__))

#define MACRO_TO_STR2(A) #A
#define MACRO_TO_STR(A) MACRO_TO_STR2(A)

#define MAX_ETH_RAW_FRAME_SIZE 4096

/* OpenRG common URLs and emails */
#define VENDOR_DOMAIN "actiontec.com"
#define VENDOR_SUPPORT_EMAIL "sales@"
#define VENDOR_SALES_EMAIL "sales@"
#define VENDOR_ANON_PWD ""
#define JUNGO_DOMAIN VENDOR_DOMAIN
#define JUNGO_SITE "www." JUNGO_DOMAIN
#define JUNGO_URL "http://" JUNGO_SITE
#define RG_SUPP_EMAIL VENDOR_SUPPORT_EMAIL JUNGO_DOMAIN
#define RG_SALES_EMAIL VENDOR_SALES_EMAIL JUNGO_DOMAIN
#define RG_ANON_USERNAME "anonymous"
#define RG_ANON_PWD VENDOR_ANON_PWD JUNGO_DOMAIN

#define MAX_LINE_SIZE 1024
#define MAX_VAR_NAME 80
#define MAX_CMD_LEN 300
#define MAX_PATH_LEN 100
#define MAX_QUERY_LEN 1024
#define MAX_IP_LEN 20
#define MAX_MAC_LEN 18
#define DOTTED_IP_LEN 16 	/* length of: xxx.xxx.xxx.xxx\0 */
#define LONG_NUM_LEN 16
#define MAX_PROC_CMD_FILENAME 32
#define MAX_DEV_FILENAME 32
#ifndef MAX_ERROR_MSG_LEN 
#define MAX_ERROR_MSG_LEN MAX_LINE_SIZE
#endif
#define MAX_8021X_KEY_CHARS 27 /* 104 bit WEP key */

/* The following are not including '\0' */
#define MAX_WORKGROUP_NAME_LEN 15 
#define MAX_DOMAIN_NAME_LEN 255 
#define MAX_HOST_NAME_LEN 63
#define MAX_SSID_LEN 32

#define MAX_USERNAME_LEN 64
#define MAX_POLICY_NAME_LEN MAX_USERNAME_LEN
/* username "@" domain */
#define MAX_EMAIL_LEN (MAX_USERNAME_LEN+1+MAX_DOMAIN_NAME_LEN)
#define MAX_PASSWORD_LEN 64
#define MAX_ENC_KEY_LEN 128
#define MAX_FULLNAME_LEN 128
/* Windows allows use of upto 256 characters for PPTP client password */
#define MAX_SECRET_LEN 256

#define MAX_URL_LEN  \
    ( \
    sizeof("http://") - 1 + \
    MAX_USERNAME_LEN + \
    sizeof(":") - 1 + \
    MAX_PASSWORD_LEN + \
    sizeof("@") - 1 + \
    MAX_HOST_NAME_LEN + \
    sizeof(":65535/") - 1 + \
    MAX_PATH_LEN + \
    MAX_QUERY_LEN \
    )
    
#define MAX_PPP_USERNAME_LEN 100
#define MAX_PPP_PASSWORD_LEN 100

#define MAX_DHCPS_LEASES 256

#define MAX_ENCRYPTION_KEY_LEN 20
#define MAX_WIRELESS_AP_KEY_MAX 26

#define MAX_CERT_SUBJECT_LEN 255
#define MAX_X509_NAME_LEN 64
#define X509_OPENRG_NAME_PREFIX "ORname_"

/* limitation defined in rp-l2tp */
#define L2TP_MAX_SECRET_LEN 96

#define MAX_DESCR_LEN 64

#include <rg_os.h>

#define PPTP_CLIENT_SUFFIX	"pptp_cli"
#define LOCAL_HOST		"localhost"

#define SYSTEM_LOG		0x1 
#define SYSTEM_DAEMON		0x2
#define SYSTEM_CLOSE_FDS	0x4

#define MAX_BAD_REBOOT 3

#define PPTPS_MAX_CONNS 10

/* echo needed to keep the fw state alive, otherwise it is deleted after two
 * minutes idle time and the connection will be blocked.
 * The default values below will make ppp discover broken link in 30
 * seconds (5*6)
 */
#ifndef CONFIG_PPP_ECHO_INTERVAL
#define PPP_ECHO_INTERVAL 6
#else
#define PPP_ECHO_INTERVAL (CONFIG_PPP_ECHO_INTERVAL)
#endif

#ifndef CONFIG_PPP_ECHO_FAILURE
#define PPP_ECHO_FAILURE 5
#else 
#define PPP_ECHO_FAILURE (CONFIG_PPP_ECHO_FAILURE)
#endif

#ifndef CONFIG_PPPOE_RETRANSMIT
#define PPPOE_RETRANSMIT 10
#else
#define PPPOE_RETRANSMIT (CONFIG_PPPOE_RETRANSMIT)
#endif

#ifndef CONFIG_PPPOE_MAX_RETRANSMIT_TIMEOUT
#define PPPOE_MAX_RETRANSMIT_TIMEOUT ULONG_MAX
#else
#define PPPOE_MAX_RETRANSMIT_TIMEOUT (CONFIG_PPPOE_MAX_RETRANSMIT_TIMEOUT)
#endif

#define M2_IF_STATUS_UP 1
#define M2_IF_STATUS_DOWN 2

#define ITF_ANY_INTERFACE ""
#define ITF_LOOPBACK "lo"

#ifdef CONFIG_RG_SSID_NAME
    #define WLAN_DEFAULT_SSID CONFIG_RG_SSID_NAME
#else
    #define WLAN_DEFAULT_SSID "openrg"
#endif

#define FW_DEFAULT_POLICY "default"
#define FW_DEFAULT_POLICY_DESC "Default Policy"
#define FW_CH_DEFAULT_POLICY "ch_default"
#define FW_CH_DEFAULT_POLICY_DESC "CableHome Default Policy"
#define FW_CH_DISABLED_POLICY "ch_disabled"
#define FW_CH_DISABLED_POLICY_DESC "CableHome disabled-filtering policy"

#define INITIAL_RULES_DESCR "Initial Rules"
#define FINAL_RULES_DESCR "Final Rules"

#define CH_ALWAYS_DESCR "Never-disabled rules"
#define CH_CONF_DESCR "Configured ruleset"
#define CH_FACTORY_DESCR "Factory default ruleset"
#define CH_DEFAULT_DESCR "General behavior rules"

/* Rule group definitions
 * Important Note:
 * ipfilter 3.x doesn't support creating more then one head for the same
 * group. 
 * when a group number is specified, ipfw will add 1 to the number if the
 * direction of the rule is outbound. 
 * Bidir rules will be added once to the defined group number, and again to the
 * incremented group number.
 */
#define GRP_DEFAULT  		0
/* Trusted interfaces configuration */
#define GRP_TRUSTED  		10 
#define GRP_TRUSTED_OUT		11 
/* Security rules: IP Spoofing, IP options, IP fragments. */
#define GRP_SECURITY 		20 
#define GRP_SECURITY_OUT	21
/* OpenRG Support: RIP, IGMP, DHCP, PPTP, IPSec etc... */
#define GRP_OPENRG		30
#define GRP_OPENRG_OUT		31
#define GRP_RG_TASKS_IN		32
#define GRP_RG_TASKS_OUT	33
/* PacketCable*/
#define GRP_PKTCBL		40
#define GRP_PKTCBL_OUT		41
#define GRP_STATIC_RNAT      	50
/* User-defined rules (grouped by svc_rule_type_t) */
#define GRP_SVC_RMT_MAN		100
#define GRP_SVC_LOCAL_MAN	110
#define GRP_SVC_LOCAL_SRV	200
#define GRP_DMZ			300
#define GRP_SVC_ACCESS_CTRL_BLOCK	400
#define GRP_SVC_ACCESS_CTRL_BLOCK_RULE	401
#define GRP_SVC_ACCESS_CTRL_ALLOW_RULE	410
#define GRP_MAC_FILT		450
#define GRP_SVC_PARENT_CTRL	500
#define GRP_SVC_PARENT_CTRL_RULE 501
#define GRP_SVC_ADVANCED	600
#define GRP_SVC_ADVANCED_OUT	601
#define GRP_IPSEC		700
#define GRP_INITIAL_INBOUND	900
#define GRP_INITIAL_OUTBOUND    1000
#define GRP_SVC_DMZ_HOST        1010
#define GRP_FINAL_INBOUND	1100
#define GRP_FINAL_OUTBOUND    	1200
#define GRP_IN			1300
#define GRP_OUT			1400
#define GRP_STATIC_NAT_OUT	1460

/* CableHome chains */
#define GRP_CH_IN_CONF		1500
#define GRP_CH_IN_FACTORY	1501
#define GRP_CH_IN_ALWAYS	1502
#define GRP_CH_IN_DEFAULT	1503
#define GRP_CH_OUT_CONF		1600
#define GRP_CH_OUT_FACTORY	1601
#define GRP_CH_OUT_ALWAYS	1602
#define GRP_CH_OUT_DEFAULT	1603

#define GRP_TRANS_PXY           1650
/* HTTP proxy group */
#define GRP_HTTP_PXY		1700

#define ALG_CHAIN		2000
#define NAT_CHAIN		2001
#define BIDIR_NAT_IN_CHAIN	2002

/* QoS chains*/
#define QOS_CHAIN_OUT 3000
#define QOS_CHAIN_IN 3001

#define	QOS_CHAIN_APP		3006
#define	QOS_CHAIN_FLOW		3008

/* DHCPS  chains*/
#define QOS_CHAIN_AUTO_OUT 4001

/* chain indexes 3002-3299 are reserved for device classless qos chains
 * (the chains that reside in /qos/chain)
 * chain indexes 3300-4000 are reserved to sticky qos-class chains */
#define BASE_STICKY_CHAINS 3300

/* Management Colors */
#define CLR_WORKING 0x008000
#define CLR_FAILURE 0xFF0000
#define CLR_DEFAULT 0x000000
#define CLR_INACTIVE 0x000080

/* upload/download rg_conf SSI defines */
#define SAVE_RG_CONF_CGI "save_rg_conf.cgi"
#define REPLACE_RG_CONF_CGI "replace_rg_conf.cgi"
#define NEW_RG_CONF "new_rg_conf"

/* HTTP variable names */
#define HTTP_VAR_ORG_URL "org_url"

/* File server definitions */
#define FSRV_MOUNT_POINT_BASE	 "/mnt/fs"
#define FSRV_MAX_SHARE_NAME_LEN	 (sizeof("_dev_sdz99")) - 1
#define FSRV_MAX_MOUNT_POINT_LEN \
    (sizeof(FSRV_MOUNT_POINT_BASE) + FSRV_MAX_SHARE_NAME_LEN)

/* UPnP */
#define UPNP_MAX_RULES		256
#define UPNP_CHECK_INTERVAL	5

#endif

