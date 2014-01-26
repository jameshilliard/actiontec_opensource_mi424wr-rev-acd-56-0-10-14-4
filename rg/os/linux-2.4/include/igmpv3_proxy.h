/*
 * IGMPv3 Proxy support
 */

#ifndef _IGMPV3_PROXY_H
#define _IGMPV3_PROXY_H

#include <linux/config.h>

#ifdef ACTION_TEC_IGMPV3

#ifdef __KERNEL__
#include <linux/if.h>
#include <linux/in.h>
#include <net/sock.h>
#else
#include <netinet/in.h>
#endif /* __KERNEL__ */


/**********************************************************************
 **********************************************************************/
/* IGMP Proxy ioctl defines */
#define IGMP_PROXY_BASE			    220

#define IGMP_PROXY_INIT			    (IGMP_PROXY_BASE + 0)
#define IGMP_PROXY_UNINIT	        (IGMP_PROXY_BASE + 1)

#define IGMP_PROXY_WAN_IF_ADD		(IGMP_PROXY_BASE + 2)
#define IGMP_PROXY_WAN_IF_DEL		(IGMP_PROXY_BASE + 3)

#define IGMP_PROXY_LAN_IF_ADD		(IGMP_PROXY_BASE + 4)
#define IGMP_PROXY_LAN_IF_DEL		(IGMP_PROXY_BASE + 5)

#define IGMP_PROXY_SET_CLIENT_UNSOL_INTERVAL    (IGMP_PROXY_BASE + 6)
#define IGMP_PROXY_GET_CLIENT_UNSOL_INTERVAL    (IGMP_PROXY_BASE + 7)

#define IGMP_PROXY_SET_QUERIER_COUNTERS         (IGMP_PROXY_BASE + 8)
#define IGMP_PROXY_GET_QUERIER_COUNTERS         (IGMP_PROXY_BASE + 9)

#define IGMP_PROXY_GET_CLIENT_VERSION	        (IGMP_PROXY_BASE + 10)
#define IGMP_PROXY_GET_STATUS	                (IGMP_PROXY_BASE + 11)

#define IGMP_PROXY_GET_CLIENT_GROUP_TABLE_SIZE	        (IGMP_PROXY_BASE + 12)
#define IGMP_PROXY_GET_CLIENT_GROUP_TABLE_NEXT_INDEX	(IGMP_PROXY_BASE + 13)
#define IGMP_PROXY_GET_CLIENT_GROUP_TABLE_ENTRY	        (IGMP_PROXY_BASE + 14)

#define IGMP_PROXY_GET_ROUTER_INTF_TABLE_SIZE	    (IGMP_PROXY_BASE + 15)
#define IGMP_PROXY_GET_ROUTER_INTF_TABLE_NEXT_INDEX	(IGMP_PROXY_BASE + 16)
#define IGMP_PROXY_GET_ROUTER_INTF_TABLE_ENTRY	(IGMP_PROXY_BASE + 17)

#define IGMP_PROXY_GET_CLIENT_GROUPSTATS_TABLE_SIZE	        (IGMP_PROXY_BASE + 18)
#define IGMP_PROXY_GET_CLIENT_GROUPSTATS_TABLE_NEXT_INDEX	(IGMP_PROXY_BASE + 19)
#define IGMP_PROXY_GET_CLIENT_GROUPSTATS_TABLE_ENTRY	    (IGMP_PROXY_BASE + 20)

#define IGMP_PROXY_GET_INCLUDE_SRC_TABLE_SIZE	    (IGMP_PROXY_BASE + 21)
#define IGMP_PROXY_GET_INCLUDE_SRC_TABLE_NEXT_INDEX	(IGMP_PROXY_BASE + 22)
#define IGMP_PROXY_GET_INCLUDE_SRC_TABLE_ENTRY	    (IGMP_PROXY_BASE + 23)

#define IGMP_PROXY_GET_EXCLUDE_SRC_TABLE_SIZE	    (IGMP_PROXY_BASE + 24)
#define IGMP_PROXY_GET_EXCLUDE_SRC_TABLE_NEXT_INDEX	(IGMP_PROXY_BASE + 25)
#define IGMP_PROXY_GET_EXCLUDE_SRC_TABLE_ENTRY	    (IGMP_PROXY_BASE + 26)

#define IGMP_PROXY_GET_ROUTER_HOST_TABLE_SIZE	    (IGMP_PROXY_BASE + 27)
#define IGMP_PROXY_GET_ROUTER_HOST_TABLE_NEXT_INDEX	(IGMP_PROXY_BASE + 28)
#define IGMP_PROXY_GET_ROUTER_HOST_TABLE_ENTRY	    (IGMP_PROXY_BASE + 29)

#define IGMP_PROXY_GET_CLIENT_GROUP_TABLE_MAXSIZE   (IGMP_PROXY_BASE + 30)
#define IGMP_PROXY_GET_ROUTER_HOST_TABLE_MAXSIZE    (IGMP_PROXY_BASE + 31)

#define IGMP_PROXY_SET_MCF                          (IGMP_PROXY_BASE + 32)

#define IGMP_PROXY_GET_GUI_GROUP_ENTRY              (IGMP_PROXY_BASE + 33)
#define IGMP_PROXY_GET_GUI_GROUPSTAT_ENTRY          (IGMP_PROXY_BASE + 34)

#define IGMP_PROXY_END                              (IGMP_PROXY_BASE + 34)


/* IGMP Router Timeout values and Counters */
#define IGMPRX_ROBUSTNESS			        2
#define IGMPRX_QUERY_INTERVAL		        125
#define IGMPRX_QUERY_RESPONSE_INTERVAL      10
#define IGMPRX_LAST_MEMBER_QUERY_INTERVAL	1

/* Max number of LAN Hosts per Multicast Group */
#define IGMPRX_MAX_HOSTS_PER_GROUP  8

/* IGMP Client Unsoicited Report Interval Range */
#define IGMPRX_CLIENT_UNSOL_REPORT_INTERVAL_MIN     1
#define IGMPRX_CLIENT_UNSOL_REPORT_INTERVAL_MAX     25

#define IGMP_PROXY_CFG_DISABLE 0
#define IGMP_PROXY_CFG_ENABLE  1


/**********************************************************************
 **********************************************************************/
/* IGMP Router's Querier Compatibility Mode */
typedef enum 
{
    IGMPRX_QCM_IGMPV1 = 1,
    IGMPRX_QCM_IGMPV2 = 2,
    IGMPRX_QCM_IGMPV3 = 3
} igmprx_querier_mode_t;

typedef enum 
{
    IGMP_STATUS_DISABLED = 0,
    IGMP_STATUS_ENABLED = 1,
    IGMP_STATUS_ERROR = 2
} igmp_status_t;


/**********************************************************************
 **********************************************************************/
/* 
 * igmprx_if_t must be passed as a parameter to:
 * - IGMP_PROXY_INIT - defualt if index/ip;
 * - IGMP_PROXY_LAN_IF_ADD/IGMP_PROXY_LAN_IF_DEL lan if index/ip
 */
struct igmprx_if_t 
{
    struct in_addr if_address;
    int if_index;
};

struct igmp_s 
{
    int control_socket;
};

typedef struct igmp_s igmp_t;

typedef struct igmp_wan_if_t
{
    struct igmp_wan_if_t *next;
    char if_name[IFNAMSIZ];
} igmp_wan_if_t;

typedef struct igmp_lan_if_t
{
    struct igmp_lan_if_t *next;
    char if_name[IFNAMSIZ];
} igmp_lan_if_t;

struct igmprx_querier_counters_t 
{
    int qcm;
    int fastleave;
    int robustness;
    int query_interval;
    int query_resp_interval;
    int startup_query_interval;
    int startup_query_count;
    int lastmember_query_interval;
    int lastmember_query_count;
};

struct igmprx_router_intf_req_t 
{
    /* Multicast Group */
    unsigned int mcgrp_addr;

    int tbl_index;

    /* Router Interface Table Entry */
    unsigned char if_desc[256];
    int ifindex;
    int uptime;
    int filter_mode;
    int include_num_entries;
    int exclude_num_entries;
    int host_num_entries;
};

struct igmprx_srec_req_t 
{
    /* Multicast Group */
    unsigned int mcgrp_addr;

    /* LAN ifIndex */
    int lan_ifindex;

    int tbl_index;
    int tbl_size;

    /* Include/Exclude Source Table Entry */
    int srec_addr;
};

struct igmprx_rtr_host_req_t 
{
    /* Multicast Group */
    unsigned int mcgrp_addr;

    /* LAN ifIndex */
    int lan_ifindex;

    int tbl_index;
    int tbl_size;

    /* Include/Exclude Source Table Entry */
    unsigned int host_addr;
    unsigned long host_last_report_time;
};

/* Multicast Filtering */
#define IGMPRX_MAX_MCF_GROUPS           32

#define IGMPRX_MCF_CREATE_UPDATE_MCGRP  1
#define IGMPRX_MCF_DESTROY_MCGRP        2

struct igmprx_mcfgrp_info
{
    unsigned int mcgrp_addr;
    unsigned int mcgrp_mask;
};

struct igmprx_mcfgrp_req
{
    char ifname[IFNAMSIZ];
    int grp_count;
    struct igmprx_mcfgrp_info grp[IGMPRX_MAX_MCF_GROUPS];
    int action;
};


/**********************************************************************
 **********************************************************************/
#ifdef __KERNEL__

#ifdef CONFIG_RG_IGMP_PROXY
int igmprx_upstream_filter(int ifindex, unsigned int grp_addr);
int igmprx_setsockopt(struct sock *sk, int optname, char *optval, int optlen);
int igmprx_getsockopt(struct sock *sk, int optname, char *optval, int *optlen);
int igmprx_input(struct sk_buff *skb);
void igmprx_if_up(struct in_device *in_dev);
void igmprx_if_down(struct in_device *in_dev);
void igmprx_recv(struct sk_buff *skb);
#endif /* CONFIG_RG_IGMP_PROXY */

#endif /* __KERNEL__ */

#endif /* ACTION_TEC_IGMPV3 */

#endif /* _IGMPV3_PROXY_H */
