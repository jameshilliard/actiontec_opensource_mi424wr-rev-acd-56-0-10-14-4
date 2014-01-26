/**************************************************************************** 
 *  Copyright (c) 2002 Jungo LTD. All Rights Reserved.
 * 
 *  /home/bat/bat/actiontec_bhr_4_0/rg/pkg/igmp/igmp_proxy.h
 *
 *  Developed by Jungo LTD.
 *  Residential Gateway Software Division
 *  www.jungo.com
 *  info@jungo.com
 *
 *  This file is part of the OpenRG Software and may not be distributed,
 *  sold, reproduced or copied in any way.
 *
 *  This copyright notice should not be removed
 *
 */

#ifndef _IGMP_PROXY_H_
#define _IGMP_PROXY_H_

#ifndef ACTION_TEC_IGMPV3

#define IGMP_PROXY_BASE			    220
#define IGMP_PROXY_INIT			    (IGMP_PROXY_BASE + 0)
#define IGMP_PROXY_UNINIT	        (IGMP_PROXY_BASE + 1)
#define IGMP_PROXY_LAN_IF_ADD		(IGMP_PROXY_BASE + 2)
#define IGMP_PROXY_LAN_IF_DEL		(IGMP_PROXY_BASE + 3)
#define IGMP_PROXY_END              (IGMP_PROXY_BASE + 3)

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


#ifdef CONFIG_RG_IGMP_PROXY
struct sock;
struct sk_buff;
struct in_device;

int igmprx_setsockopt(struct sock *sk, int optname, char *optval, int optlen);
int igmprx_getsockopt(struct sock *sk, int optname, char *optval, int *optlen);
int igmprx_input(struct sk_buff *skb);
void igmprx_if_up(struct in_device *in_dev);
void igmprx_if_down(struct in_device *in_dev);
void igmprx_recv(struct sk_buff *skb);
#endif /* CONFIG_RG_IGMP_PROXY */

#endif /* ACTION_TEC_IGMPV3 */

#endif /* _IGMP_PROXY_H_ */
