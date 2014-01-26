/*
 *      Internet Group Management Protocol Version 3 (IGMPv3)
 *
 *      This code implements the IGMPv3 protocol as defined in 
 *      draft-ietf-idmr-igmp-v3-11.txt.  IGMPv3 is backwards compatible with 
 *      IGMPv2 as defined in RFC 2236 and with IGMPv1 as defined in RFC 1112.  
 *      To enable IGMPv3, set CONFIG_IP_MULTICAST and CONFIG_IP_IGMPV3.
 *
 *	Version: $Id: igmpv3.c,v 1.19 2009/07/13 16:57:45 athill Exp $
 * 
 *	Authors:
 *		Wilbert de Graaf (wilbert@kloosterhof.com)	(FreeBSD impl)
 *		Vince Laviano (vlaviano@cisco.com)		(Linux port)
 *
 *	The majority of this code is derived from the FreeBSD IGMPv3
 *	implementation by Wilbert de Graaf (wilbert@kloosterhof.com).  See
 *	http://www.kloosterhof.com/~wilbert/igmpv3.html for details.
 *
 *      Code was also derived from the earlier Linux IGMPv2 implementation,
 *      still available in igmpv2.c, by Alan Cox, Chih-Jen Chang, Tsu-Sheng
 *      Tsao, Christian Daudt, Malcolm Beattie, and Alexey Kuznetsov.
 *
 *      Cisco would like to acknowledge the work of Sprint Labs towards
 *      IGMPv3 for Linux, which they have made publically available at
 *      www.sprintlabs.com/Department/IP-Interworking/multicast/linux-igmpv3/
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 *
 *      Portions of this code are also licensed under the following terms:
 *
 *      "The FreeBSD Copyright
 *
 *      Copyright 1994-2002 FreeBSD, Inc. All rights reserved.
 *
 *      Redistribution and use in source and binary forms, with or without
 *      modification, are permitted provided that the following conditions
 *      are met:
 *
 *      1. Redistributions of source code must retain the above copyright
 *         notice, this list of conditions and the following disclaimer.
 *      2. Redistributions in binary form must reproduce the above copyright
 *         notice, this list of conditions and the following disclaimer in the
 *         documentation and/or other materials provided with the distribution.
 *
 *      THIS SOFTWARE IS PROVIDED BY THE FREEBSD PROJECT ``AS IS'' AND ANY
 *      EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *      IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *      PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE FREEBSD PROJECT OR
 *      CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *      EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *      PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *      PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *      LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *      NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *      SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *      The views and conclusions contained in the software and documentation
 *      are those of the authors and should not be interpreted as representing
 *      official policies, either expressed or implied, of the FreeBSD Project
 *      or FreeBSD, Inc."
 */

#include <linux/config.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/inetdevice.h>
#include <linux/igmp.h>
#include <linux/if_arp.h>
#include <linux/rtnetlink.h>
#include <linux/proc_fs.h>
#include <net/ip.h>
#include <net/protocol.h>
#include <net/route.h>
#include <net/sock.h>
#include <net/checksum.h>
#include <linux/netfilter_ipv4.h>
#ifdef CONFIG_IP_MROUTE
#include <linux/mroute.h>
#endif

#ifdef CONFIG_RG_IGMP_PROXY
#include <igmp/igmp_proxy.h>
#endif /* CONFIG_RG_IGMP_PROXY */
#ifdef CONFIG_RG_IGMP_PROXY_MODULE
#include <linux/module.h>
int (*igmp_proxy_upstream_filter)(int ifindex, unsigned int group_addr);
void (*igmp_proxy_recv)(struct sk_buff *skb);
void (*igmp_proxy_if_up)(struct in_device *in_dev);
void (*igmp_proxy_if_down)(struct in_device *in_dev);
EXPORT_SYMBOL(igmp_proxy_upstream_filter);
EXPORT_SYMBOL(igmp_proxy_recv);
EXPORT_SYMBOL(igmp_proxy_if_up);
EXPORT_SYMBOL(igmp_proxy_if_down);
#endif /* CONFIG_RG_IGMP_PROXY_MODULE */

#undef IGMPV3_HOST_DEBUG

#ifdef IGMPV3_HOST_DEBUG
#define igmp_printk printk
#else
#define igmp_printk(fmt, args...)
#endif


/*
 * Only compile the contents of this file if IGMPv3 is configured.  Otherwise,
 * fall back to the original IGMPv2 implementation contained in igmpv2.c
 */
#ifdef ACTION_TEC_IGMPV3

/*
 * New definitions and macros
 */
#define PR_FASTHZ			(5)	    /* 5 fast timeouts per sec */
#define PR_SLOWHZ			(2) 	/* 2 slow timeouts per sec */	
#define IGMP_FASTTIMER_INTERVAL		(HZ/PR_FASTHZ)	/* 200 msecs */
#define IGMP_SLOWTIMER_INTERVAL		(HZ/PR_SLOWHZ)	/* 500 msecs */

/*
 * A DoS protection against successive group-and-source specific (gass)
 * queries.  Successive means queries that affect the same response.
 */
#define DEFAULT_MAX_MEMBERSHIPS 	128
#define DEFAULT_MAX_GASS_QUERIES	8
#define DEFAULT_MAX_REC_SOURCES		64
#define DEFAULT_DEBUG_OUTPUT		0

/*
 * When new source-filters are needed, allocate a few of them at once
 */
#define SF_NUMALLOC	8

/*
 * Assigning a timer that is handled by the fast_timer procedure
 */
#define ASSIGN_FAST_TIMER(t, v)	 	{ (t) = (v); igmp_ft_dirty = 1; }

/*
 * Some address tests
 */
#define IS_IGMP_ALL_HOSTS(x)	((x.s_addr) == IGMP_ALL_HOSTS)

#define IGMP_VALID_SFADDR(a)\
	(((a).s_addr != INADDR_ANY) && ((a).s_addr != INADDR_NONE) &&\
	(!MULTICAST((a).s_addr)))

#define IPV4SPACE(x)	((x) << (2))

/*
 * Make sf the start of the 'fmode' list of 'inm'
 */
#define SF_START(fmode, inm, sf)\
	(sf)=((fmode)==MCAST_EXCLUDE)?(inm)->inm_sflexcl:(inm)->inm_sflincl;

#define SF_FIND(ip, start, sf)\
    if (start)\
    {\
        for ((sf)=(start);(sf)!=NULL; (sf)=(sf)->isf_next) {\
            if ((sf)->isf_addr.s_addr == ip.s_addr) break;\
            else if ((sf)->isf_addr.s_addr>ip.s_addr) { (sf)=NULL; break;}}\
    }

/*
 * Clear the corresponding bits of 'mask' in all the tags, starting from start
 */
#define SF_CLEARTAG(start, sf, mask)\
    if (start)\
    {\
        for ((sf)=(start);(sf)!=NULL;(sf)=((sf)->isf_next))\
            (sf)->isf_reporttag &= ~(mask);\
    }

/*
 * Of entries from 'start' that have 'mask' set, set the bits of 'smask' and
 * clear them of 'cmask', in all the tags.
 */
#define SF_SETCLEARTAG(start, sf, mask, smask, cmask)\
    if (start)\
    {\
        for ((sf)=(start);(sf)!=NULL;(sf)=((sf)->isf_next)){\
            if (((sf)->isf_reporttag & (mask)) == (mask)) {\
                (sf)->isf_reporttag |= (smask);\
                (sf)->isf_reporttag &= ~(cmask);\
            }\
        }\
    }

/*
 * Determine whether a list, starting from 'start' is empty
 */
#define SF_ISEMPTY(start, sf, is)\
	(is)=1;\
    if (start)\
    {\
        for ((sf)=(start);(sf)!=NULL;(sf)=(sf)->isf_next)\
            if ((sf)->isf_refcount != 0) { (is)=0; break; }\
    }

/* 
 * Expand LIST_HEAD macro
 */
struct in_multihead 
{
	struct in_multi *lh_first;
} in_multihead;

/*
 * Macro for looking up the in_multi record for a given IP multicast address
 * on a given interface.  If no matching record is found, "inm" is set null.
 * Things are simplified compared to BSD code, because we've eliminated
 * struct ifmultiaddr.
 */
#define IN_LOOKUP_MULTI(addr, ifp, inm) \
	/* struct in_addr addr; */ \
	/* struct in_device *ifp; */ \
	/* struct in_multi *inm; */ \
	do { \
        if ((ifp)->mc_list)\
        {\
            for ((inm) = (ifp)->mc_list; (inm); \
                 (inm) = (inm)->inm_next) { \
                if ((inm)->inm_addr.s_addr == (addr).s_addr) { \
                    break; \
                } \
            } \
        }\
	} while(0)

/*
 * Macro to step through all of the in_multi records, one at a time.
 * The current position is remembered in 'step', which the caller must
 * provide.  IN_FIRST_MULTI(), below, must be called to initialize 'step'
 * and get the first record.  Both macros return a NULL 'inm' when there
 * are no remaining records.
 */
#define IN_NEXT_MULTI(step, inm)\
	/* struct in_multistep step; */\
	/* struct in_multi *inm; */\
	do {\
		if (((inm) = (step).i_inm) != NULL)\
			(step).i_inm = (step).i_inm->inm_link.le_next;\
	} while (0)

#define IN_FIRST_MULTI(step, inm)\
	/* struct in_multistep step; */\
	/* struct in_multi *inm; */\
	do {\
		(step).i_inm = in_multihead.lh_first;\
		IN_NEXT_MULTI((step), (inm));\
	} while (0)

#define LIST_REMOVE(elm, field) \
	do { \
		if ((elm)->field.le_next != NULL) \
			(elm)->field.le_next->field.le_prev = \
                            (elm)->field.le_prev; \
		*(elm)->field.le_prev = (elm)->field.le_next; \
	} while (0)

#define LIST_INSERT_HEAD(head, elm, field) \
	do { \
	    if (((elm)->field.le_next = (head)->lh_first) != NULL) \
		    (head)->lh_first->field.le_prev = &(elm)->field.le_next; \
	    (head)->lh_first = (elm); \
	    (elm)->field.le_prev = &(head)->lh_first; \
	} while (0)



/***********************************************************************
 * GLOBAL  VARIABLES
 **********************************************************************/
spinlock_t igmp_spinlock = SPIN_LOCK_UNLOCKED;  
pid_t igmp_spinlock_pid = -1; /* -1 for none, else pid */
unsigned igmp_spinlock_level = 0;


/***********************************************************************
 * GLOBAL FUNCTION PROTOTYPES
 **********************************************************************/


/***********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **********************************************************************/

static void __igmp_print_inmp(struct in_multi *inmp);

static inline char *rti_version_string(int version);
static inline char *ip_mc_fmode_string(unsigned short fmode);
static inline int output_maybe_reroute(struct sk_buff *skb);

static void igmp_sendpkt(struct in_multi *inm, 
                         int type, 
                         unsigned long addr);

static struct in_sfentry *igmp_get_sources(struct igmp_grouprec *group, 
	struct in_sfentry *slist, unsigned short mask, int n, int index, 
	int rex, int dec, int *cnt);
static void igmp_send_report(struct in_multi *inm, 
                             struct in_sfentry *slist, 
                             int count, 
                             unsigned short mask, 
                             unsigned char type, 
                             unsigned long addr);
static int check_router_alert(struct iphdr *ip);

static struct router_info *find_or_create_rti(struct in_device *ifp);
static void free_rti(struct in_device *ifp);
static struct router_info *get_rti(struct in_device *ifp);
static void put_rti(struct in_device *ifp);

static int igmp_random_delay(int mrt, int ver);
static int igmp_record_sources(struct in_multi *inmp, 
                               unsigned int numsrc, 
                               struct in_addr *sources);
static void igmp_clean_recorded_sources(struct in_multi *inmp);
static void igmp_schedule_curstate_reports(struct in_device *ifp, 
                                           struct router_info *rti, 
                                           struct igmpv3 *igmp, 
                                           int igmplen, 
                                           int mrt, 
                                           struct sk_buff *skb);

static struct in_sfentry* igmp_sf_new(struct in_multi *inmp);
static int igmp_sf_add(struct in_multi *inmp, struct in_addr ia, int fmode);
static void igmp_sf_remove(struct in_multi *inmp, struct in_addr ia, int fmode);
static void igmp_sf_unlink_unreferenced(struct in_multi *inmp, int fmode);
static void igmp_free_all_sourcefilters(struct in_multi *inmp);

static struct in_multi *in_createmulti(struct in_addr *ap, struct in_device *ifp);
static void igmp_unlink_intf_inm(struct in_multi *inm);
static void in_removemulti(struct in_multi *inm);
static void in_destroymulti(struct in_multi *inm);

static int igmp_tag_state(struct in_multi *inmp, 
                          int *fmode, 
                          struct in_sfentry **taggedlist, 
                          int userec, 
                          unsigned short tagmask);
static int igmp_tag_srclist_change(struct in_sfentry *isf, 
                                   unsigned short maskreport, 
                                   unsigned short masktime, 
                                   int xmit);
static int igmp_chg_sourcefilter(struct in_multi *inmp, 
                                 struct source_list *src_rem, 
                                 int fmode_from, 
                                 struct source_list *src_add, 
                                 int fmode_to);

static void igmp_send_if_report(struct in_device *ifp);
static void igmp_current_state_report(struct in_multi *inmp, int userec);
static void igmp_send_slc_report(struct in_multi *inm, unsigned long addr);
static int igmp_state_change_report(struct in_multi *inmp, int *done);

static void igmp_cancel_all_reports(int cancel_statechg_too);
static void igmp_fasttimo(unsigned long data);
static void igmp_slowtimo(unsigned long data);

static void igmp_joingroup(struct in_multi *inm);
static void igmp_leavegroup(struct in_multi *inmp);

static void ip_mc_filter_add(struct in_device *in_dev, u32 addr);
static void ip_mc_filter_del(struct in_device *in_dev, u32 addr);

static int ip_addmembership(struct sock *sk, struct in_addr *group, 
                     struct in_device *ifp, int *index);
static int ip_findmembership(struct sock *sk, struct in_addr *group, 
                      struct in_device *ifp, int *index);
static int ip_dropmembership(struct sock *sk, struct in_addr *group, 
 		      struct in_device *ifp);
static int ip_get_sopt_mreq_arg(int optname, char *optval, int optlen,
    struct in_addr *gaddr, struct in_addr *saddr, struct in_device **ifpp);

static int ip_sock_add_msf(struct sock_mcastsf *msf, struct in_addr src);
static int ip_sock_remove_msf(struct sock_mcastsf *msf, struct in_addr src);

static int ip_mc_procinfo(char *buffer, char **start, off_t offset, 
                          int length, int *eof, void *data);
static int ip_mc_stats(char *buffer, char **start, off_t offset, 
                       int length, int *eof, void *data);

/***********************************************************************
 * Static global data
 **********************************************************************/
static int igmp_ft_count;	/* fasttimer processing */
static int igmp_ft_dirty; 
static int igmp_ft_stamp;	
static struct timer_list igmp_fasttimer;
static struct timer_list igmp_slowtimer;
static struct igmpstat igmpstat;
static struct router_info *Head;


struct proc_dir_entry *igmp_proc_dir;

/* Sysctl variables */
int sysctl_igmp_max_memberships = DEFAULT_MAX_MEMBERSHIPS;
int sysctl_igmp_max_gass_queries = DEFAULT_MAX_GASS_QUERIES;
int sysctl_igmp_max_rec_sources = DEFAULT_MAX_REC_SOURCES;
int sysctl_igmp_debug_output = DEFAULT_DEBUG_OUTPUT;


int igmp_host_version = IGMP_V3_HOST;

static int igmp_host_unsol_interval = IGMP_UNSOL_INT;
#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
static int igmp_persistent_join_interval = IGMP_UNSOL_INT;
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */


/***********************************************************************
 **********************************************************************/
static void __igmp_print_inmp(struct in_multi *inmp)
{
    if (inmp != NULL)
	    igmp_printk(KERN_DEBUG "__igmp_print_inmp: inmp=%p  users=%d  \
                        Group=0x%08x  Mode=%s  (%p:%p)\n",
				inmp, inmp->users, inmp->inm_addr.s_addr, 
                ip_mc_fmode_string(inmp->inm_fmode),
                (inmp)?inmp->inm_sflincl:0, (inmp)?inmp->inm_sflexcl:0);
}

/*
 * Convert rti_type field, stored as type value for the corresponding
 * IGMP report, into a string indicating IGMP version. 
 */ 
static inline char *rti_version_string(int version)
{
	switch (version) 
    {
	    case IGMP_V1_MEMBERSHIP_REPORT:
		return "v1";
	    case IGMP_V2_MEMBERSHIP_REPORT:
		return "v2";
	    case IGMP_V3_MEMBERSHIP_REPORT:
		return "v3";
	    default:
		return "unknown!";
	}
}

/*
 * Convert filter mode value into an explanatory string.
 */
static inline char *ip_mc_fmode_string(unsigned short fmode)
{
	switch (fmode) 
    {
	    case MCAST_INCLUDE:
		return "INCLUDE";
	    case MCAST_EXCLUDE:
		return "EXCLUDE";
	    default:
		return "unknown!";
	}
}

/* 
 * Return 1 if 'addr' falls in this range:
 * 		224.0.0.0/24
 * 		239.0.0.0/24
 */
static inline int igmp_reserved_address(u32 addr)
{
    return ((addr & 0xFF000000) == 0xE0000000) || ((addr & 0xFF000000) == 0xEF000000);
}

/* 
 * Don't just hand NF_HOOK skb->dst->output, in case 
 * netfilter hook changes route 
 */
static inline int output_maybe_reroute(struct sk_buff *skb)
{
	return skb->dst->output(skb);
}

/*
 * Send out an igmp (v1, v2) report for membership 'inm'
 */
static void igmp_sendpkt(struct in_multi *inm, 
                         int type, 
                         unsigned long addr)
{
	struct net_device *dev;
	struct sk_buff *skb;
	struct iphdr *ip;
	struct igmphdr *igmp;
	struct rtable *rt;
	u32 dst;

	igmp_printk(KERN_DEBUG "igmp_sendpkt\n");

	dst = (addr != 0) ? addr : inm->inm_addr.s_addr; 
	dev = inm->inm_ifp->dev;

#ifdef CONFIG_RG_IGMP_PROXY
    if (!igmprx_upstream_filter(dev->ifindex, dst))
        return;
#endif /* CONFIG_RG_IGMP_PROXY */
#ifdef CONFIG_RG_IGMP_PROXY_MODULE
    if (igmp_proxy_upstream_filter)
    {
        if (!(igmp_proxy_upstream_filter(dev->ifindex, dst)))
            return;
    }
#endif /* CONFIG_RG_IGMP_PROXY_MODULE */

	if (ip_route_output(&rt, dst, 0, 0, dev->ifindex)) 
    {
		igmp_printk(KERN_DEBUG "igmp_sendpkt: ip_route_output\n");
		return;
	}
	if (rt->rt_src == 0) 
    {
		ip_rt_put(rt);
		igmp_printk(KERN_DEBUG "igmp_sendpkt: rt->rt_src == 0\n");
		return;
	}
	
	skb = alloc_skb(sizeof(struct iphdr) + 4 + IGMP_MINLEN + 
                        dev->hard_header_len + 15, GFP_ATOMIC);
	if (skb == NULL) 
    {
		ip_rt_put(rt);
		igmp_printk(KERN_DEBUG "igmp_sendpkt: NULL skb\n");
		return;
	}
	skb->dst = &rt->u.dst;
	skb_reserve(skb, (dev->hard_header_len + 15) & ~15);

	skb->nh.iph = ip = (struct iphdr *) skb_put(skb, 
		sizeof(struct iphdr) + 4);

	igmp = (struct igmphdr *) skb_put(skb, IGMP_MINLEN);
	igmp->type = type;
	igmp->code = 0;
	igmp->group = inm->inm_addr.s_addr;
	igmp->csum = 0;
	igmp->csum = ip_compute_csum((void *) igmp, IGMP_MINLEN);

	ip->version = 4;
	ip->ihl = (sizeof(struct iphdr) + 4) >> 2;
	ip->tos = 0;
	ip->frag_off = __constant_htons(IP_DF);
	ip->ttl = 1;
	ip->saddr = rt->rt_src;
	ip->daddr = dst;
	ip->protocol = IPPROTO_IGMP;
	ip->tot_len = htons(sizeof(struct iphdr) + 4 + IGMP_MINLEN);
	ip_select_ident(ip, &rt->u.dst, NULL);
	/*
	 * Construct router alert option
	 */	
	((u8 *) &ip[1])[0] = IPOPT_RA;
	((u8 *) &ip[1])[1] = 4;
	((u8 *) &ip[1])[2] = 0;
	((u8 *) &ip[1])[3] = 0;
	ip_send_check(ip);	
		
	/*
	 * Send the report
	 */
	NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, rt->u.dst.dev,
		output_maybe_reroute);

    if (inm->inm_rpt_statechg > 0)
    {
    	--inm->inm_rpt_statechg;
        ASSIGN_FAST_TIMER(inm->inm_timer,
                        igmp_host_unsol_interval*PR_FASTHZ);
    }
#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
    else
    {
        if (inm->inm_startup_statechg_report)
        {
            ASSIGN_FAST_TIMER(inm->inm_timer,
                            igmp_persistent_join_interval*PR_FASTHZ);
        }
    }
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */

    if (type == IGMP_V1_MEMBERSHIP_REPORT)
    {
	    igmpstat.igps_snd_v1_joins++;	
        igmp_printk(KERN_DEBUG "igmp_sendpkt: Sent IGMPv1 Join successfully for Group=0x%08x\n", 
                    inm->inm_addr.s_addr);
    }
    else if (type == IGMP_V2_MEMBERSHIP_REPORT)
    {
	    igmpstat.igps_snd_v2_joins++;	
        igmp_printk(KERN_DEBUG "igmp_sendpkt: Sent IGMPv2 Join successfully for Group=0x%08x\n", 
                    inm->inm_addr.s_addr);
    }
    else if (type == IGMP_V2_LEAVE_GROUP)
    {
	    igmpstat.igps_snd_v2_leaves++;	
        igmp_printk(KERN_DEBUG "igmp_sendpkt: Sent IGMPv2 Leave successfully for Group=0x%08x\n", 
                    inm->inm_addr.s_addr);
    }
    else
    {
        igmp_printk(KERN_DEBUG "igmp_sendpkt: Sent unknown IGMPv2 message successfully for Group=0x%08x\n", 
                    inm->inm_addr.s_addr);
    }

	igmpstat.igps_snd_reports++;	
}

/*
 * Store at most 'n' sources from 'slist' that match 'mask' into 'group' and
 * return the number that have been stored, and the address of the source that
 * needs to be reported next.
 *
 * If 'rex' then deal with retransmission state
 */
static struct in_sfentry *igmp_get_sources(struct igmp_grouprec *group, 
	struct in_sfentry *slist, unsigned short mask, int n, int index, 
	int rex, int dec, int *cnt)	
{
	int i;
	
	for (i = index; ((i < n) && (slist != NULL)); slist = slist->isf_next) 
    {
        if (!slist)
            break;

		if ((slist->isf_reporttag & mask) == mask) 
        {
			if (rex && slist->isf_rexmit == 0) 
            {
				continue;
			}
			if (dec && slist->isf_rexmit > 0) 
            {
				slist->isf_rexmit--;
			}
			group->ig_sources[i++] = slist->isf_addr.s_addr;
		}
	}
	*cnt = i - index;
	return (slist == NULL) ? NULL : slist->isf_next;	
}


/*
 * Send an IGMP report.
 * inm - Interface Multicast Group info
 * slist - Source Address list
 * count - Number of Source Addresses
 * mask - Used to select the Source addresses from slist to be reported
 * type - IGMPv3 Report type
 * addr - Multicast Group address
 */
static void igmp_send_report(struct in_multi *inm, 
                             struct in_sfentry *slist, 
                             int count, 
                             unsigned short mask, 
                             unsigned char type, 
                             unsigned long addr)
{
	struct sk_buff *skb;
	struct iphdr *ip;
	struct rtable *rt;
	struct igmp_report *report;
	struct igmp_grouprec *group;
	int n, nsrcs, hlen, len, did, limit, rex, dec;
	struct net_device *dev;
	u32 dst;

	igmp_printk(KERN_DEBUG "igmp_send_report\n");

	dst = (addr) ? addr : htonl(INADDR_ALLRTRS_IGMPV3_GROUP); 
	dev = inm->inm_ifp->dev;

#ifdef CONFIG_RG_IGMP_PROXY
    if (!igmprx_upstream_filter(dev->ifindex, inm->inm_addr.s_addr))
        return;
#endif /* CONFIG_RG_IGMP_PROXY */
#ifdef CONFIG_RG_IGMP_PROXY_MODULE
    if (igmp_proxy_upstream_filter)
    {
        if (!(igmp_proxy_upstream_filter(dev->ifindex, inm->inm_addr.s_addr)))
            return;
    }
#endif /* CONFIG_RG_IGMP_PROXY_MODULE */
	/*
	 * Determine some useful values for message size
	 */
	hlen = sizeof(struct iphdr) + 4 + IGMP_HDRLEN + IGMP_GRPREC_HDRLEN +
		IGMP_PREPEND;
	limit = inm->inm_ifp->dev->mtu;

	/*
 	 * If it's about exclude, report only as many as fit in one message
	 */
    /* CHANGE_FROM_ACTION_TEC */
	if ((limit < (IPV4SPACE(count) + hlen)) &&
		(type == IGMP_REPORT_MODE_EX || type == IGMP_REPORT_TO_EX)) {
		count = (limit - hlen) / sizeof(struct in_addr);
	}

	/*
	 * Update retransmission state in case of to_xx report
	 */
	rex = 0;	
	dec = (type == IGMP_REPORT_TO_EX || type == IGMP_REPORT_TO_IN) ? 1 : 0;

	/* 
	 * Send out the report, and, if necessary, use multiple datagrams
	 */
	did = 0;
	do 
    {
		if (ip_route_output(&rt, dst, 0, 0, dev->ifindex)) 
        {
		    igmp_printk(KERN_DEBUG "igmp_send_report: ip_route_output\n");
		    return;
		}
		if (rt->rt_src == 0) 
        {
		    ip_rt_put(rt);
		    igmp_printk(KERN_DEBUG "igmp_send_report: rt->rt_src == 0\n");
		    return;
		}

		/*
		 * Allocate mtu-sized skbuff
		 */
		skb = alloc_skb(dev->mtu + dev->hard_header_len+15, GFP_ATOMIC);
		if (skb == NULL) 
        {
			ip_rt_put(rt);
			igmp_printk(KERN_DEBUG "igmp_send_report: NULL skb\n");
			return;
		}
		skb->dst = &rt->u.dst;
		skb_reserve(skb, (dev->hard_header_len + 15) & ~15);
		skb->nh.iph = ip = 
            (struct iphdr *) skb_put(skb, sizeof(struct iphdr) + 4);
		report = (struct igmp_report *) skb_put(skb, IGMP_HDRLEN);

		nsrcs = (limit - hlen) >> 2; 		

		/*
		 * Find out how many sources we can report in this message and
		 * its size.
		 */	
		if (nsrcs > count) 
        {
			nsrcs = count;
		}
		len = IGMP_HDRLEN + IGMP_GRPREC_HDRLEN + IPV4SPACE(nsrcs);	

		report->ir_type = (u_char) IGMP_V3_MEMBERSHIP_REPORT;
		report->ir_rsv1 = 0;
		report->ir_numgrps = htons(1);
		report->ir_rsv2 = htons(0);

		/*
		 * Set the igmp group record header and put the sources in the
		 * buffer
		 */
		group = &(report->ir_groups[0]);
		slist = 
            igmp_get_sources(group, slist, mask, nsrcs, 0, rex, dec,&n);
		report->ir_groups[0].ig_type = (u_char) type;
		report->ir_groups[0].ig_datalen = 0;
		report->ir_groups[0].ig_numsrc = htons(n);
		report->ir_groups[0].ig_group = inm->inm_addr.s_addr;

		/*
		 * Tell skbuff how much group record data we've stored.  We've
		 * already accounted for the ip header, the router alert option,
		 * and the report header.
		 */
		skb_put(skb, (IGMP_GRPREC_HDRLEN + n * sizeof(struct in_addr))); 

		/*
		 * Compute igmp checksum and set the ip header
		 */
		report->ir_cksum = 0;
		report->ir_cksum = ip_compute_csum((void *) report, len);

		ip->version = 4;
		ip->ihl = (sizeof(struct iphdr) + 4) >> 2;
		ip->tos = 0;
		ip->frag_off = __constant_htons(IP_DF);
		ip->ttl = 1;
		ip->saddr = rt->rt_src;
		ip->daddr = dst;
		ip->protocol = IPPROTO_IGMP;
		ip->tot_len = htons(len + sizeof(struct iphdr) + 4);
		ip_select_ident(ip, &rt->u.dst, NULL);
		/*
		 * Construct router alert option
		 */
		((u8 *) &ip[1])[0] = IPOPT_RA; 
		((u8 *) &ip[1])[1] = 4;
		((u8 *) &ip[1])[2] = 0;
		((u8 *) &ip[1])[3] = 0;
		ip_send_check(ip);

		/* send the report */
		NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, rt->u.dst.dev,
		        output_maybe_reroute);

	    switch (type)
        {
            case IGMP_REPORT_MODE_IN:
		        igmpstat.igps_snd_v3_mode_in_reports++;
                break;
            case IGMP_REPORT_MODE_EX:
		        igmpstat.igps_snd_v3_mode_ex_reports++;
                break;
            case IGMP_REPORT_TO_IN:
		        igmpstat.igps_snd_v3_to_in_reports++;
                break;
            case IGMP_REPORT_TO_EX:
		        igmpstat.igps_snd_v3_to_ex_reports++;
                break;
            case IGMP_REPORT_ALLOW_NEW:
		        igmpstat.igps_snd_v3_allow_reports++;
                break;
            case IGMP_REPORT_BLOCK_OLD:
		        igmpstat.igps_snd_v3_block_reports++;
                break;
        }
		igmpstat.igps_snd_v3_reports++;
		igmpstat.igps_snd_reports++;
        igmp_printk(KERN_DEBUG "igmp_send_report: Sent IGMPv3 Report (type=%d) successfully for Group=0x%08x\n", 
                    type, inm->inm_addr.s_addr);

		did += nsrcs;	

	} while ((did < count) && (nsrcs != 0) && (slist != NULL));
}

/*
 * Return whether the IP router alert option has been set, which is required
 * for IGMPv3 queries.
 */
static int check_router_alert(struct iphdr *ip)
{
	int opt, optlen, cnt;
	unsigned char *cp;

	igmp_printk(KERN_DEBUG "check_router_alert\n");

	cp = (unsigned char *) (ip + 1);
	cnt = (ip->ihl << 2) - sizeof(*ip);
	for (; cnt > 0; cnt -= optlen, cp += optlen) {
		opt = cp[IPOPT_OPTVAL];
		if (opt == IPOPT_EOL) {
			break;
		}
		if (opt == IPOPT_NOP) {
			optlen = 1;
		} else {
			optlen = cp[IPOPT_OLEN];
			if (optlen <= 0 || optlen > cnt) {
				break;
			}
			if (opt == IPOPT_RA) {
				return 1;
			}
		}
	}
	return 0;
}

/*
 * Find or build a router information record corresponding to an 
 * interface address
 * Caller must call this function under igmp_lock 
 */
static struct router_info *find_or_create_rti(struct in_device *ifp)
{
	struct router_info *rti;

    if (Head != NULL)
    {
        for (rti = Head; rti; rti = rti->rti_next) 
            if (rti->rti_ifp == ifp)
            {
                rti->rti_refcnt++;
                return rti;
            }
    }

	/*
	 * No information yet, so create a new router information record and
	 * initialize
	 */
	rti = (struct router_info *) kmalloc(sizeof(*rti), GFP_ATOMIC);
	if (rti == NULL) 
    {
		/*
		 * BSD code doesn't seem to allow for this malloc to fail. It
		 * uses the MALLOC() macro and then immediately dereferences
		 * rti.  Is the memory somehow guaranteed to be available?  
		 */
		igmp_printk(KERN_DEBUG "find_or_create_rti: kmalloc failed for rti\n");
		return NULL;
	}	

	rti->rti_ifp = ifp;
	in_dev_hold(ifp);
	rti->rti_timer = rti->rti_timev1 = rti->rti_timev2 = 0;
	rti->rti_type = IGMP_V3_ROUTER;
	rti->rti_qrv = IGMP_INIT_ROBVAR;
	rti->rti_next = Head;
	Head = rti;
    rti->rti_refcnt=1;

    igmp_printk("find_or_create_rti: allocated rti=%p\n", rti);
	return rti;	
}	

/* Caller must call this function under igmp_lock */
static void free_rti(struct in_device *ifp)
{
	struct router_info *rti, *prev_rti, *tmp_rti;

    if (!Head)
        return;

    prev_rti = NULL;
	for (rti = Head; rti; rti = rti->rti_next) 
    {
		if (rti->rti_ifp == ifp)
            break;
        prev_rti = rti;
    }

    if (!rti)
        return;

    if (rti->rti_refcnt > 0)
        return;

    /* Remove from list */
    tmp_rti = rti;
    rti = rti->rti_next;

    if (!prev_rti)
        Head = rti;
    else
        prev_rti->rti_next = rti;

	tmp_rti->rti_timer = tmp_rti->rti_timev1 = tmp_rti->rti_timev2 = 0;
	in_dev_put(tmp_rti->rti_ifp);

    igmp_printk("free_rti: rti=%p\n", tmp_rti);
    kfree(tmp_rti);
}

/* Caller must call this function under igmp_lock */
static struct router_info *get_rti(struct in_device *ifp)
{
	struct router_info *rti = NULL;

    if (!Head)
        return NULL;

    for (rti = Head; rti; rti = rti->rti_next) 
    {
        if (rti->rti_ifp == ifp)
        {
            rti->rti_refcnt++;
            return rti;
        }
    }

    return NULL;
}	

static void put_rti(struct in_device *ifp)
{
	struct router_info *rti = NULL;

    if (!Head)
        return;

    for (rti = Head; rti; rti = rti->rti_next) 
    {
        if (rti->rti_ifp == ifp)
        {
            rti->rti_refcnt--;
            if (rti->rti_refcnt <= 0)
            {
                free_rti(ifp);
            }
            break;
        }
    }
}

/*
 * Determine a random delay given some 'maximum response time'. If version is
 * IGMPv3, the IGMPv3 formula will be used to compute the delay.
 */
static int igmp_random_delay(int mrt, int ver)
{
	if (ver == IGMP_V3_ROUTER && (mrt & 0x80)) 
    {
		/*
		 * XXX Might be good to give the formula embodied in the
		 * following line.
		 */
		mrt = (0x10 | (mrt & 0x0f)) << (((mrt & 0x70) >> 4) + 3);
	}
	return IGMP_RANDOM_DELAY(mrt);
}

/*
 * Record sources
 */
static int igmp_record_sources(struct in_multi *inmp, 
                               unsigned int numsrc, 
                               struct in_addr *sources)
{
	struct in_sfentry *prev = NULL, *cur, *isf;
	int i, j, count = inmp->inm_num_rec_sources;
	struct in_addr *srcs = sources;	

	/*
 	 * Don't record anything if the maximum is reached.
	 */
	if (sysctl_igmp_max_rec_sources > 0 && 
	    sysctl_igmp_max_rec_sources <= count) 
    {
		return 0;
	}

    igmp_printk("igmp_record_sources\n");
	cur = NULL;
	for (i = j = 0; i < numsrc; i++) 
    {
		if (IGMP_VALID_SFADDR(srcs[j])) 
        {
			/*
			 * Insert the address in the (sorted) list of recorded
			 * sources. Test if the sources are sorted and, if not,
			 * start looking for a new position from start.
			 */
			if (cur == NULL || 
			    cur->isf_addr.s_addr > srcs[j].s_addr) {
				cur = inmp->inm_sflrec;
				prev = NULL;
			}
			while (cur != NULL) {
				if (cur->isf_addr.s_addr >= srcs[j].s_addr) {
					break;
				}
				prev = cur;
				cur = cur->isf_next;
			}
			if (cur == NULL || cur->isf_addr.s_addr != 
			    srcs[j].s_addr) {
				if ((isf = igmp_sf_new(inmp)) != NULL) {
					isf->isf_addr.s_addr = srcs[j].s_addr;
					isf->isf_next = cur;
					if (prev != NULL) {
						prev->isf_next = isf;
					} else {
						inmp->inm_sflrec = isf;
					}
					count++;
					cur = isf;
					if (sysctl_igmp_max_rec_sources > 0 &&
					    sysctl_igmp_max_rec_sources <= count) {
						break;
					}
				}	
			}
		}
		j++;
	} 	
	inmp->inm_num_rec_sources += count;
	return (inmp->inm_num_rec_sources);	
}

/*
 * Clean the recorded sources
 */
static void igmp_clean_recorded_sources(struct in_multi *inmp)
{
	struct in_sfentry *isf = inmp->inm_sflrec;

    igmp_printk("igmp_clean_recorded_sources\n");
	inmp->inm_sflrec = NULL;
	inmp->inm_num_rec_sources = 0;
    if (isf)
    {
        for (; isf != NULL; isf = isf->isf_next) 
        {
            isf->isf_addr.s_addr = INADDR_NONE;
        }
    }
}

/*
 * Schedule reports in response to a single query. The value of 'mrt' is the
 * igmp 'maximum response time' that should be used for the response.
 */
static void igmp_schedule_curstate_reports(struct in_device *ifp, 
                                           struct router_info *rti, 
                                           struct igmpv3 *igmp, 
                                           int igmplen, 
                                           int mrt, 
                                           struct sk_buff *skb)
{
	struct in_multi *inm;
	struct in_multistep step;
	int *ptimer, num_sources, oldtimer, delay, ignore;
	
	igmp_printk(KERN_DEBUG "igmp_schedule_curstate_reports\n");

	/*
	 * Select a random delay
	 */
	delay = igmp_random_delay(mrt, rti->rti_type);
	delay = (delay * PR_FASTHZ) / IGMP_TIMER_SCALE;

	/*
	 * Ignore query if it's a v3 router and there is a pending response to
	 * a general query scheduled to be sent sooner than our selected delay.
	 */
	if (rti->rti_type == IGMP_V3_ROUTER && 
        rti->rti_timer && 
	    rti->rti_timer < delay) 
    {
		return;
	}

	/*
	 * Schedule reports by setting a timer in all of our membership records
	 * to which the query applies, and if the interface is the same.
	 */
	IN_FIRST_MULTI(step, inm);
	while (inm != NULL) 
    {
		/*
         * Don't schedule responses if the membership's interface
	   	 * doesn't match the one that received the query, or if the
		 * membership is for the all hosts group or applies to a
		 * loopback interface.
		 */
		if (inm->inm_ifp == ifp && 
            inm->inm_addr.s_addr != IGMP_ALL_HOSTS &&
		    (inm->inm_ifp->dev->flags & IFF_LOOPBACK) == 0) 
        {
			if (igmp->igmp_group == 0) 
            {
				/*
				 * General query: schedule report if there is
				 * no pending timer, or pending timer > delay.
				 * For v3 routers, the action is different (an
				 * interface timer).
				 */
				if (rti->rti_type == IGMP_V3_ROUTER) 
                {
					if (rti->rti_timer <= 0 ||
					    rti->rti_timer > delay) 
					{
					    ASSIGN_FAST_TIMER(rti->rti_timer,
							      delay);
					}
					break;
				} 
                else if (inm->inm_timer == 0 ||
					   inm->inm_timer > delay) 
                {
					igmp_printk(KERN_DEBUG "igmp_schedule_curstate_reports: General Query - 0x%08x  inm_timer=%d\n",
								inm->inm_addr.s_addr, inm->inm_timer);
					ASSIGN_FAST_TIMER(inm->inm_timer,delay);
					delay = igmp_random_delay(mrt, 
					    rti->rti_type);
					delay = delay * PR_FASTHZ / 
					    IGMP_TIMER_SCALE;
				}

			} 
            else if (igmp->igmp_group == inm->inm_addr.s_addr) 
            {
				/*
				 * Group-specific, or group-and-source specific
				 * query
				 */
				if (igmplen >= 12) 
                {
					/*
					 * Group-specific or group-and-source
					 * v3 query
					 */
					num_sources = ntohs(igmp->igmp_numsrc);
					ptimer = (num_sources < 1) ? 
				 	    &inm->inm_timer :
					    &inm->inm_ti_curstate;
					if (inm->inm_timer == 0 &&
					    inm->inm_ti_curstate == 0) 
                    {
					    /*
					     * No pending response
					     * Vince: Huh? Didn't we just check
					     *     this?
					     */
					    inm->inm_timer = 0;
					    inm->inm_ti_curstate = 0;
					    if (num_sources > 0) 
                        {
					        inm->inm_num_gass_queries = 1;
					        igmp_record_sources(inm,
						    num_sources, 
						    (struct in_addr *)
						    igmp->igmp_sources);
					    }
						igmp_printk(KERN_DEBUG "igmp_schedule_curstate_reports: v3 GS Query - 0x%08x  inm_timer=%d\n",
								inm->inm_addr.s_addr, inm->inm_timer);
					    ASSIGN_FAST_TIMER(*ptimer, delay);
				    } 
                    else 
                    {
					    /*
					     * There is a pending response
					     */
					    ignore = 0;
					    if (num_sources == 0 ||
					        inm->inm_timer != 0) {
				        /*
						 * group-and-source-specific
						 * reports are overruled by
						 * group-specific reports
						 */
					       ptimer = &inm->inm_timer;
					       igmp_clean_recorded_sources(inm);
					    } else {
						/*
				         * Record sources unless the
						 * number of gass queries 
						 * exceeds a certain threshold
						 * (accessible through sysctl())
						 * or if this protection is
						 * disabled (<0)
						 */
						if (sysctl_igmp_max_gass_queries<=0 ||
						  sysctl_igmp_max_gass_queries >
						  ++inm->inm_num_gass_queries) {
						    igmp_record_sources(inm,
						       num_sources, 
						       (struct in_addr *)
						       igmp->igmp_sources);
						} else {
						    ignore = 1;
						}
					    }
					    /*
					     * Select earliest of pending timer
					     * and new delay
					     */
					    if (!ignore) {
					        oldtimer = *ptimer;
						inm->inm_timer = 0;
						inm->inm_ti_curstate = 0;
						if (delay < oldtimer) {
						    ASSIGN_FAST_TIMER(*ptimer,
								      delay);
						} else {
						    ASSIGN_FAST_TIMER(*ptimer,
								      oldtimer);
						}
					    }
                    }
				} 
				else if (igmplen == 8) 
				{
					/*
					 * Group-specific v2 query: schedule a
					 * new report if there is no pending
					 * report for this group, or if there
					 * is, if its scheduled time is later
					 * than the new time.
					 */
					if (inm->inm_timer == 0 ||
					    inm->inm_timer > delay) {
					       inm->inm_timer = 0;
				  	       igmp_clean_recorded_sources(inm);
						igmp_printk(KERN_DEBUG "igmp_schedule_curstate_reports: v2 GS Query - 0x%08x  inm_timer=%d\n",
								inm->inm_addr.s_addr, inm->inm_timer);
					       ASSIGN_FAST_TIMER(inm->inm_timer,
								 delay);
					}
				}
			
				/*
				 * No other membership record can match the 
				 * interface and group-address, so stop.
				 */
				break;
			}

		}
		IN_NEXT_MULTI(step, inm);
	} /* while (inm) */
}

/* 
 * Create a new sourcefilter record and add it to the in_multi's pool
 */
static struct in_sfentry* igmp_sf_new(struct in_multi *inmp)
{
	struct in_sfentry *isf, *pool;
	int n;

	/*
 	 * First try to find a free entry
	 */
	for (isf = inmp->inm_sflpool; isf != NULL; isf = isf->isf_pnext) 
    {
		if (isf->isf_addr.s_addr == INADDR_NONE) 
        {
			isf->isf_addr.s_addr = INADDR_ANY;
			return isf;
		}
	}

	/*
 	 * Need to allocate some new source-filter entries
	 */
	for (n = 0; n < SF_NUMALLOC; n++) 
    {
		/*
		 * Use ATOMIC flag since bottom halves are disabled
		 */
		isf = (struct in_sfentry *) kmalloc(sizeof *isf, GFP_ATOMIC);
		if (isf != NULL) 
        {
			/*
			 * Initialize fields
			 */
			isf->isf_next = isf->isf_pnext = NULL;
			isf->isf_addr.s_addr = INADDR_ANY;
			
			/*
			 * Add it to the pool
			 */
			if ((pool = inmp->inm_sflpool) != NULL) 
            {
				isf->isf_pnext = pool;
				inmp->inm_sflpool = isf;
			} 
            else 
            {
				inmp->inm_sflpool = isf;
			}
		}
	}
	return isf;
}

/*
 * Add an ip address to an in_multi's source filter
 */
static int igmp_sf_add(struct in_multi *inmp, struct in_addr ia, int fmode)
{
	struct in_sfentry *isf=NULL, *start, *sf, *sf2;

	/*
	 * See if there is an entry, and, if so, increment refcount
	 */
	SF_START(fmode, inmp, start);
	SF_FIND(ia, start, isf);
	if (isf != NULL) 
    {
		isf->isf_refcount++;
		return 0;
	}

	/*
	 * Have to find/create a new entry (in the pool)
	 */
	if ((isf = igmp_sf_new(inmp)) == NULL) 
    {
		return -ENOMEM;
	}

	/*
	 * Initialize
	 */
	isf->isf_addr.s_addr = ia.s_addr;
	isf->isf_refcount = 1;
	isf->isf_reporttag = 0;
	isf->isf_rexmit = 0;
	isf->isf_next = NULL;
	
	/*
	 * Insert it (sorted) into the proper 'fmode' list
	 */
	sf2 = NULL;
	for (sf = start; sf && sf->isf_addr.s_addr < ia.s_addr; 
             sf = sf->isf_next) 
    {
		sf2 = sf;
	}
	if (sf2 == NULL) 
    {
		isf->isf_next = sf;
		if (fmode == MCAST_EXCLUDE) 
        {
			inmp->inm_sflexcl = isf;
		} 
        else 
        {
			inmp->inm_sflincl = isf;
		} 
	} 
    else 
    {
		isf->isf_next = sf2->isf_next;
		sf2->isf_next = isf;
	}

	return 0;
}

/*
 * Remove an ip address from an in_multi's source filter by decreasing the
 * refcount
 */
static void igmp_sf_remove(struct in_multi *inmp, struct in_addr ia, int fmode)
{
	struct in_sfentry *isf;

	SF_START(fmode, inmp, isf);
	SF_FIND(ia, isf, isf);
	if (isf != NULL && isf->isf_refcount != 0) 
    {
		isf->isf_refcount--;
	}
}

/*
 * Unlink all unreferenced entries from a 'fmode' source filter list
 */
static void igmp_sf_unlink_unreferenced(struct in_multi *inmp, int fmode)
{
	struct in_sfentry **list, *isf, *isf2 = NULL;

	list = (fmode == MCAST_EXCLUDE) ? &inmp->inm_sflexcl : &inmp->inm_sflincl;

    if (*list)
    {
        for (isf = *list; isf != NULL; isf = isf->isf_next) 
        {
            if (isf->isf_refcount == 0 && isf->isf_rexmit == 0) 
            {
                if (isf2 != NULL) 
                {
                    isf2->isf_next = isf->isf_next;
                    isf2 = isf;
                } 
                else 
                {
                    *list = isf->isf_next;
                }
                isf->isf_addr.s_addr = INADDR_NONE;
            } 
            else 
            {
                isf2 = isf;
            }
        }
    }
}

/*
 * Free all the sourcelists and empty the pool of some membership
 */
static void igmp_free_all_sourcefilters(struct in_multi *inmp) 
{

	struct in_sfentry *isf, *isf2;

	inmp->inm_sflexcl = NULL;
	inmp->inm_sflincl = NULL;
	inmp->inm_sflrec = NULL;
	
	isf = inmp->inm_sflpool;
	inmp->inm_sflpool = NULL;
	while (isf != NULL) 
    {
		isf2 = isf;
		isf = isf->isf_pnext;
		kfree(isf2);
	}
}

/*
 * Create a group record for a given group and interface
 * Differ a bit from FreeBSD code here, to avoid rewriting dev layer.
 */
static struct in_multi *in_createmulti(struct in_addr *ap, struct in_device *ifp)
{
	struct in_multi *inm;

    igmp_printk(KERN_DEBUG "in_createmulti: dev=%s    group=%08x\n", 
                ifp->dev->name, ap->s_addr);

    read_lock(&ifp->lock);
    if (ifp->mc_list)
    {
        for (inm = ifp->mc_list; inm; inm = inm->inm_next) 
        {
            if (inm->inm_addr.s_addr == ap->s_addr) 
            {
                inm->users++;	
                read_unlock(&ifp->lock);
                igmp_printk(KERN_DEBUG "in_createmulti: dev=%s    group=%08x    users=%d\n", 
                                        ifp->dev->name, ap->s_addr, inm->users);
                return inm;
            }	
        }
    }
    read_unlock(&ifp->lock);

	if (ifp->mcgrp_count >= DEFAULT_MAX_MEMBERSHIPS)
		return NULL;

 	inm = (struct in_multi *) kmalloc(sizeof(*inm), GFP_ATOMIC);  /* XXX */
	if (!inm) 
    {
		return NULL;
    }	

	memset(inm, 0, sizeof(*inm));
    inm->users = 1;
	inm->loaded = 1;
	inm->inm_addr = *ap;	
	in_dev_hold(ifp);
	inm->inm_ifp = ifp;
	inm->inm_fmode = MCAST_INCLUDE;
	inm->inm_state = IGMP_NONEXISTENT;
	inm->inm_sflexcl = NULL;
	inm->inm_sflincl = NULL;
	inm->inm_sflrec = NULL;
	inm->inm_sflpool = NULL;
	inm->inm_num_socks_excl = 0;
	inm->inm_num_rec_sources = 0;
	inm->inm_ti_statechg_interval == igmp_host_unsol_interval;

    inm->inm_start_total_stats = jiffies;

    /* Place on per interface multicast list */
    write_lock_bh(&ifp->lock);
	inm->inm_next = ifp->mc_list;
	ifp->mc_list = inm;
    ++ifp->mcgrp_count;
    write_unlock_bh(&ifp->lock);

	/*
	 * Place inm on global in_multihead list
	 */
	LIST_INSERT_HEAD(&in_multihead, inm, inm_link);

	/* Let IGMP know that we have joined a new IP multicast group */
	igmp_joingroup(inm);

	/* If interface is up, tell routing about join event */
	if (ifp->dev->flags & IFF_UP)
		ip_rt_multicast_event(ifp);
	
    igmp_printk(KERN_DEBUG "in_createmulti: dev=%s    group=%08x    inm=%p\n", 
                            ifp->dev->name, ap->s_addr, inm);
	return inm; 	
}

/*
 * Add an address to the list of IP multicast addresses for a given interface.
 */
struct in_multi *in_addmulti(struct in_addr *ap, struct in_device *ifp)
{
	struct in_multi *inm;

	igmp_lock();
	if ((inm = in_createmulti(ap, ifp)) != NULL) 
    {
        struct timeval tv;

		inm->inm_state &= ~IGMP_NONEXISTENT;
		inm->inm_sflexcl = NULL;
		inm->inm_fmode = MCAST_EXCLUDE;
		inm->inm_num_socks_excl++;

        do_gettimeofday(&tv);
        inm->inm_last_report_time = tv.tv_sec;
        ++inm->inm_grp_self;
    }

	igmp_unlock();

    igmp_printk(KERN_DEBUG "in_addmulti: dev=%s    group=%08x    inm=%p\n", 
                            ifp->dev->name, ap->s_addr, inm);
	return (inm);
}

static void igmp_unlink_intf_inm(struct in_multi *inm)
{
    struct in_multi **inmp;

    /*
     * Remove the inm from interface's inm list here, but don't remove it 
     * from the global list or deallocate it until later.
     */
    write_lock_bh(&inm->inm_ifp->lock);
    for (inmp = &inm->inm_ifp->mc_list; *inmp; inmp = &(*inmp)->inm_next) 
    {
        if (*inmp == NULL)
            break;

        if (*inmp == inm) 
        {
            --inm->inm_ifp->mcgrp_count;
            *inmp = (*inmp)->inm_next;	
            break;
        }	
    }	
    write_unlock_bh(&inm->inm_ifp->lock);
}

static void in_removemulti(struct in_multi *inm)
{
    igmp_printk(KERN_DEBUG "in_removemulti: inm=%p\n", inm);
    if (!inm)
        return;

	/* Only delete inm if reference count drops to zero */
	if (inm->users > 0) 
	    return;

    put_rti(inm->inm_ifp);

    igmp_free_all_sourcefilters(inm);

    in_dev_put(inm->inm_ifp);

    kfree(inm); 
}

static void in_cleanupmulti(struct in_multi *inm)
{
    igmp_printk(KERN_DEBUG "in_cleanupmulti: inm=%p\n", inm);

    if (!inm)
        return;

	--inm->users;
	/* Only delete inm if reference count is greater than zero */
	if (inm->users > 0) 
	    return;

#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
    inm->inm_startup_statechg_report = 0;
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */
	inm->inm_state |= IGMP_NONEXISTENT;
	igmp_leavegroup(inm);

    /*
     * Tell routing what's happened
     */
    if (inm->inm_ifp->dev->flags & IFF_UP)
        ip_rt_multicast_event(inm->inm_ifp);

	/*
	 * Cleanup membership record unless the router is v3, since 
	 * then the record will be deleted after the final state-change
	 * report.	
	 */
	if (inm->inm_rti->rti_type != IGMP_V3_ROUTER)
    {
        LIST_REMOVE(inm, inm_link);

        in_removemulti(inm);
    }
}

/*
 * Delete a multicast address record.
 * Again, a little different than FreeBSD.
 */
static void in_destroymulti(struct in_multi *inm)
{
    igmp_printk(KERN_DEBUG "in_destroymulti: inm=%p\n", inm);

    if (!inm)
        return;

	--inm->users;
	/* Only delete inm if reference count is greater than zero */
	if (inm->users > 0) 
	    return;

#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
    inm->inm_startup_statechg_report = 0;
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */
	inm->inm_state |= IGMP_NONEXISTENT;
	igmp_leavegroup(inm);

    /*
     * Tell routing what's happened
     */
    if (inm->inm_ifp->dev->flags & IFF_UP)
        ip_rt_multicast_event(inm->inm_ifp);

	/*
	 * Cleanup membership record unless the router is v3, since 
	 * then the record will be deleted after the final state-change
	 * report.	
	 */
	if (inm->inm_rti->rti_type != IGMP_V3_ROUTER)
    {
        /* Remove from global multicast list */
        LIST_REMOVE(inm, inm_link);

        /* Remove from interface multicast list */
        igmp_unlink_intf_inm(inm);

        in_removemulti(inm);
    }
}

void in_delmulti(struct in_addr *ap, struct in_device *ifp)
{
	struct in_multi *inm = NULL;

    igmp_printk(KERN_DEBUG "in_delmulti: dev=%s    group=0x%08x\n", 
                            ifp->dev->name, ap->s_addr);

	igmp_lock();

    read_lock(&ifp->lock);
    if (ifp->mc_list)
    {
        for (inm = ifp->mc_list; inm; inm = inm->inm_next) 
        {
            if (inm->inm_addr.s_addr == ap->s_addr) 
            {
                break;
            }	
        }
    }
    read_unlock(&ifp->lock);

    if (!inm)
    {
	    igmp_unlock();
        return;
    }
    igmp_printk(KERN_DEBUG "in_delmulti: dev=%s    group=0x%08x    inm=%p(%d)\n", 
                            ifp->dev->name, ap->s_addr, inm, inm->users);

	--inm->users;
	if (inm->users > 0) 
    {
	    igmp_unlock();
        return;
    }

    --inm->inm_grp_self;
	inm->inm_state = IGMP_NONEXISTENT;
    inm->inm_sflexcl = NULL;
    inm->inm_sflincl = NULL;
    inm->inm_fmode = MCAST_INCLUDE;
    inm->inm_num_socks_excl--;

	igmp_leavegroup(inm);

    /*
     * Tell routing what's happened
     */
    if (inm->inm_ifp->dev->flags & IFF_UP)
        ip_rt_multicast_event(inm->inm_ifp);

	/*
	 * Cleanup membership record unless the router is v3, since 
	 * then the record will be deleted after the final state-change
	 * report.	
	 */
    /* Remove from global multicast list */
    LIST_REMOVE(inm, inm_link);

    /* Remove from interface multicast list */
    igmp_unlink_intf_inm(inm);

    in_removemulti(inm);

	igmp_unlock();
}


/*
 * Tag sources in one of the three lists (include, exclude, or recorded) that
 * need to be reported in either a current-state report or a change-state
 * report.
 */
static int igmp_tag_state(struct in_multi *inmp, 
                          int *fmode, 
                          struct in_sfentry **taggedlist, 
                          int userec, 
                          unsigned short tagmask)
{
	struct in_sfentry *inc, *exc, *rec;
	int count = 0;

	igmp_printk(KERN_DEBUG "igmp_tag_state\n");

	/*
 	 * Two cases: don't or do use recorded sources.
	 *
	 * Note: because all 3 lists are sorted, to match the elements it's
	 * enough to go through each list only once.
	 */
	if (userec == 0 || inmp->inm_sflrec == NULL) 
    {
		/*
		 * Don't use recorded sources.  If the interface state is
		 * exclude, tag (EXCL-INCL), else tag (INCL)
		 */
		if (inmp->inm_fmode == MCAST_EXCLUDE) 
        {
			/*
			 * Tag (EXCL - INCL)
			 */
			inc = inmp->inm_sflincl;
            if (inmp->inm_sflexcl)
            {
                for (exc=inmp->inm_sflexcl; exc; exc = exc->isf_next) 
                {
                    if ((exc->isf_refcount != 0) && 
                        (exc->isf_refcount == inmp->inm_num_socks_excl)) 
                    {
                        while (inc && 
                               inc->isf_addr.s_addr < exc->isf_addr.s_addr) 
                        {
                            inc = inc->isf_next;
                        }

                        if (inc && inc->isf_refcount != 0 &&
                            inc->isf_addr.s_addr == 
                            exc->isf_addr.s_addr) 
                        {
                            exc->isf_reporttag &= ~tagmask;
                        } 
                        else 
                        {
                            exc->isf_reporttag |= tagmask;
                            count++;
                        }
                    } 
                    else 
                    {
                        exc->isf_reporttag &= ~tagmask;
                    }
                }
            }
			*fmode = MCAST_EXCLUDE;
			*taggedlist = inmp->inm_sflexcl;
		} 
        else 
        {
			/*
			 * Tag (INCL)
			 */
            if (inmp->inm_sflincl)
            {
			    for (inc=inmp->inm_sflincl; inc; inc = inc->isf_next) 
                {
                    if (inc->isf_refcount != 0) 
                    {
                        inc->isf_reporttag |= tagmask;
                        count++;
                    } 
                    else 
                    {
                        inc->isf_reporttag &= ~tagmask;
                    }
                }
            }
			*fmode = MCAST_INCLUDE;
			*taggedlist = inmp->inm_sflincl;
		}
	} 
    else 
    {
		/*
		 * Do use recorded sources.  If interface state is exclude,
		 * tag (REC - (EX - IN)), else tag (REC * INC).
		 */
		if (inmp->inm_fmode == MCAST_EXCLUDE) 
        {
			/*
			 * Tag (REC - (EX - IN))
			 */
			exc = inmp->inm_sflexcl;
			inc = inmp->inm_sflincl;
            if (inmp->inm_sflrec)
            {
			    for (rec = inmp->inm_sflrec; rec; rec = rec->isf_next) 
                {
                    while (exc && exc->isf_addr.s_addr <
                           rec->isf_addr.s_addr) 
                    {
                        exc = exc->isf_next;
                    }
                    if (exc && exc->isf_refcount > 0 &&
                        exc->isf_addr.s_addr == rec->isf_addr.s_addr
                        && exc->isf_refcount == 
                        inmp->inm_num_socks_excl) 
                    {
                        while (inc && inc->isf_addr.s_addr <
                               exc->isf_addr.s_addr) 
                        {
                            inc = inc->isf_next;
                        }
                        if (inc && inc->isf_refcount > 0 &&
                            inc->isf_addr.s_addr == 
                            exc->isf_addr.s_addr) 
                        {
                            rec->isf_reporttag |= tagmask;
                            count++;
                        } else 
                        {
                            rec->isf_reporttag &= ~tagmask;
                        }
                    } 
                    else 
                    {
                        rec->isf_reporttag |= tagmask;
                        count++;
                    }
                }
            }
			*fmode = MCAST_INCLUDE;
			*taggedlist = inmp->inm_sflrec;
		} 
        else 
        {
			/*
			 * Tag (REC * INC) 
			 */
			rec = inmp->inm_sflrec;
            if (inmp->inm_sflincl)
            {
			    for (inc = inmp->inm_sflincl; inc; inc=inc->isf_next) 
                {
                    if (inc->isf_refcount != 0) 
                    {
                        while (rec && rec->isf_addr.s_addr <
                               inc->isf_addr.s_addr) 
                        {
                            rec = rec->isf_next;
                        }
                        if (rec && rec->isf_addr.s_addr ==
                            inc->isf_addr.s_addr) 
                        {
                            inc->isf_reporttag |= tagmask;
                            count++;
                        } 
                        else 
                        {
                            inc->isf_reporttag &= ~tagmask;
                        }
                    }
                }
            }
			*fmode = MCAST_INCLUDE;
			*taggedlist = inmp->inm_sflincl;
		}
	}
	return count;
}

/*
 * Tag the changes in a sourcelist, by comparing a state transition between T1
 * and T2, according to 'masktime' (x01x means it has been added, while x10x
 * means it has been removed). 'maskreport' is the mask that will be set and
 * 'xmit' is the new value of the retransmission counter.
 */
static int igmp_tag_srclist_change(struct in_sfentry *isf, 
                                   unsigned short maskreport, 
                                   unsigned short masktime, 
                                   int xmit) 
{
	int count = 0;

	igmp_printk(KERN_DEBUG "igmp_tag_srclist_change\n");

	while (isf != NULL) 
    {
		if ((isf->isf_reporttag & IGMP_MASK_STATE_TX) == masktime) 
        {
			isf->isf_reporttag &= ~(IGMP_MASK_BLOCK_OLD |
			    IGMP_MASK_ALLOW_NEW);
			isf->isf_reporttag |= maskreport;
			isf->isf_rexmit = xmit;
			count++;
		}
		isf = isf->isf_next;
	}
	return count;
}	

/*
 * Should be called whenever the source filter on a socket is changed. This
 * function will maintain global membership state and schedule the necessary
 * igmp state change reports.  The global membership state of the system is
 * implemented using reference counting.  Sources will only be removed if
 * their reference count becomes zero, and a global filter mode change will
 * only happen if this was the first or last socket in exclude mode.  The
 * filter mode of the socket before the change should be passed in 'fmode_from'
 * and the new filter mode in 'fmode_to'.
 *
 * The sources that are removed from the socket's source list in 'fmode_from'
 * should be passed in 'src_rem'.  Those sources that are added to its list in
 * 'fmode_to' are passed in 'src_add'.
 */
static int igmp_chg_sourcefilter(struct in_multi *inmp, 
                                 struct source_list *src_rem, 
                                 int fmode_from, 
                                 struct source_list *src_add, 
                                 int fmode_to)
{
	struct in_sfentry *isf, *list1, *list2;
	struct source_list *psrc;
	int fmode1, fmode2, cnt1, cnt2, empty, initial;

	igmp_printk(KERN_DEBUG "igmp_chg_sourcefilter\n");

	/*
 	 * Verify input.  In the initial state, a remove is invalid, as is
	 * inc{0}.
	 */
	initial = (inmp->inm_state & IGMP_NONEXISTENT);
	if (initial && 
        (src_rem != NULL || (src_add == NULL && fmode_to == MCAST_INCLUDE))) 
    {
		return 0;
	}

	/*
	 * Determine current (T1) state unless it's the initial state
	 */
	if (!initial) 
    {
		cnt1 = igmp_tag_state(inmp, &fmode1, &list1, 0, 
                                      IGMP_MASK_STATE_T1);	
	} 
    else 
    {
		fmode1 = MCAST_INCLUDE;
		cnt1 = 0;
	}

	/*
	 * Remove and/or add sources
	 */
    if (src_rem)
    {
        for (psrc = src_rem; psrc != NULL; psrc = psrc->src_next) 
        {
            igmp_sf_remove(inmp, psrc->src_addr, fmode_from);
        }
    }

    if (src_add)
    {
        for (psrc = src_add; psrc != NULL; psrc = psrc->src_next) 
        {
            igmp_sf_add(inmp, psrc->src_addr, fmode_to);
        }
    }

	/*
	 * Update reference count of sockets in exclude mode
	 */
	if (fmode_to != fmode_from) 
    {
		inmp->inm_num_socks_excl += ((fmode_to == MCAST_EXCLUDE) 
		    ? 1 : -1);
	}

	/*
	 * Determine new mode
	 */
	SF_ISEMPTY(inmp->inm_sflexcl, isf, empty);
	if (empty) 
    {
		SF_ISEMPTY(inmp->inm_sflincl, isf, empty);
		if (!empty) 
        {
			inmp->inm_fmode = MCAST_INCLUDE;
		} 
        else
        {
            if (initial)
            {
			    inmp->inm_fmode = MCAST_EXCLUDE;
            }
            else
            {
			    inmp->inm_fmode = fmode_to;
            }
		}
	} 
    else 
    {
		inmp->inm_fmode = MCAST_EXCLUDE;
	}

	/*
 	 * Whatever happened, now the state is valid.
	 */
	inmp->inm_state &= ~IGMP_NONEXISTENT;

	/*
 	 * Determine new (T2) state
	 */
	cnt2 = igmp_tag_state(inmp, &fmode2, &list2, 0, IGMP_MASK_STATE_T2);

	/*
	 * Don't bother sending reports if there isn't an IGMPv3 router, or if
	 * it would be in reference to the all_hosts group or the loopback 
	 * interface
	 */
	if ((inmp->inm_rti->rti_type < IGMP_V3_ROUTER) ||
	    (inmp->inm_addr.s_addr == IGMP_ALL_HOSTS) ||
	    (inmp->inm_ifp->dev->flags & IFF_LOOPBACK)) 
    {
		igmp_sf_unlink_unreferenced(inmp, fmode_from);
		return 1;
	}

	/*
	 * Determine state-change report
	 */
	if (fmode1 != fmode2) 
    {
		/*
		 * Generate/tag the filter-mode-changes 
		 */
		if (fmode2 == MCAST_EXCLUDE) 
        {
			SF_SETCLEARTAG(list2, isf, IGMP_MASK_STATE_T2,
			 	       IGMP_MASK_TO_EX, IGMP_MASK_TO_IN);
		} 
        else 
        {
			SF_SETCLEARTAG(list2, isf, IGMP_MASK_STATE_T2,
				       IGMP_MASK_TO_IN, IGMP_MASK_TO_EX);
		}

		/*
	 	 * Remove sources that were deleted and don't need to be
		 * reported
		 */
		if (src_rem != NULL) 
        {
			igmp_sf_unlink_unreferenced(inmp, fmode_from);
		}

		/*
		 * Schedule filter-mode-change reports
		 */
		inmp->inm_rpt_toxx = inmp->inm_rti->rti_qrv;
		inmp->inm_rpt_statechg = inmp->inm_rti->rti_qrv;
#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
		inmp->inm_ti_statechg_interval = igmp_host_unsol_interval;
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */
		ASSIGN_FAST_TIMER(inmp->inm_ti_statechg, 1);
	} 
    else 
    {
		/*
		 * Source-list changes only affect the list as reported in 
		 * current-state reports, and not the filter-mode itself.  
		 * For that reason, source-list changes can be determined by
		 * reporting the difference in this list only.
		 *
		 * Added means T1=0 and T2=1, while deleted is T1=1 and T2=0
		 * BLOCK means either deleted from INCL or added to EXCL
		 * ALLOW means either deleted from EXCL or added to INCL
		 */
		cnt1 = 0;
		if (fmode2 == MCAST_EXCLUDE) 
        {
			cnt1 = igmp_tag_srclist_change(list2, 
			           IGMP_MASK_ALLOW_NEW,	IGMP_MASK_STATE_T1,
				   inmp->inm_rti->rti_qrv);
			cnt1 += igmp_tag_srclist_change(list2,
				   IGMP_MASK_BLOCK_OLD, IGMP_MASK_STATE_T2,
				   inmp->inm_rti->rti_qrv);
		} 
        else 
        {
			cnt1 = igmp_tag_srclist_change(list2, 
				   IGMP_MASK_ALLOW_NEW, IGMP_MASK_STATE_T2,
				   inmp->inm_rti->rti_qrv);
			cnt1 += igmp_tag_srclist_change(list2,
				   IGMP_MASK_BLOCK_OLD, IGMP_MASK_STATE_T1,
				   inmp->inm_rti->rti_qrv);
		}

		/*
		 * If there are sources to report, activate the state-change
		 * timer
		 */
		if (cnt1 != 0) 
        {
			inmp->inm_rpt_statechg = inmp->inm_rti->rti_qrv;
#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
			inmp->inm_ti_statechg_interval = igmp_host_unsol_interval;
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */
			ASSIGN_FAST_TIMER(inmp->inm_ti_statechg, 1);
		}
	}

	return 1;
}

/*
 * Send out a report for a specific interface, where all memberships on this
 * interface should be reported.
 */
static void igmp_send_if_report(struct in_device *ifp)
{
    int v3_mode_in_reports=0, v3_mode_ex_reports=0;
	struct in_multi *inm;
	struct in_multistep step;
	struct igmp_report *report;
	struct igmp_grouprec *group;
	struct iphdr *ip;
	int n, left, done, bytesavail, xmit, tothlen, size;
	int ngroups, fmode, igmplen, grouplen;
	struct in_sfentry *slist;
	struct rtable *rt; 
	struct net_device *dev;
	struct sk_buff *skb;
	u32 dst;

	igmp_printk(KERN_DEBUG "igmp_send_if_report\n");

	/*
	 * Find the first membership that needs to be reported
	 */
	IN_FIRST_MULTI(step, inm);
	while (
           inm && 
           (
             IS_IGMP_ALL_HOSTS(inm->inm_addr) || 
             (ifp!=inm->inm_ifp)
#ifdef CONFIG_RG_IGMP_PROXY
             || (!igmprx_upstream_filter(ifp->dev->ifindex, inm->inm_addr.s_addr))
#endif /* CONFIG_RG_IGMP_PROXY */
#ifdef CONFIG_RG_IGMP_PROXY_MODULE
             || 
             ( 
              igmp_proxy_upstream_filter &&
              (!(igmp_proxy_upstream_filter(ifp->dev->ifindex, inm->inm_addr.s_addr)))
             )
#endif /* CONFIG_RG_IGMP_PROXY_MODULE */
           )
          ) 
    {
		IN_NEXT_MULTI(step, inm);
	}
	if (inm == NULL) 
		return;

	left = igmp_tag_state(inm, &fmode, &slist, 0, IGMP_MASK_IF_STATE);
	dst = htonl(INADDR_ALLRTRS_IGMPV3_GROUP);
	dev = ifp->dev;

	/*
	 * Loop initialization
	 */
	size = dev->mtu;
	tothlen = sizeof(*ip) + 4 + IGMP_HDRLEN + IGMP_PREPEND;
	done = bytesavail = ngroups = igmplen = 0;
	group = NULL;
	report = NULL;
	ip = NULL;
	skb = NULL;

	/* 
	 * Report all memberships on this interface
	 */
	while (!done) 
    {
		xmit = 0;

		/*
		 * If there is no more space to report available, get and 
		 * prepare a new buffer
		 */
		if (bytesavail < (IGMP_GRPREC_HDRLEN + IPV4SPACE(left))) 
        {
			/*
			 * Allocate skbuff
			 */
			skb = alloc_skb(dev->mtu + dev->hard_header_len + 15,
				        GFP_ATOMIC);
			if (skb == NULL) 
            {
				igmp_printk(KERN_DEBUG "igmp_send_if_report: "
				       "NULL skb\n");
				return;
			}
			
			bytesavail = size - tothlen;
			igmplen = IGMP_HDRLEN;

			skb_reserve(skb, (dev->hard_header_len + 15) & ~15);
			skb->nh.iph = ip = (struct iphdr *) skb_put(skb, 
				sizeof(struct iphdr) + 4);
			report = (struct igmp_report *) skb_put(skb, 
				IGMP_HDRLEN);
			group = &(report->ir_groups[0]);
			ngroups = 0;	
		}

		/*
		 * Construct the group record
		 */
		n = (bytesavail - IGMP_GRPREC_HDRLEN) / sizeof(struct in_addr);
		if (n > left) 
        {
			n = left;
		}
		slist = igmp_get_sources(group, slist, IGMP_MASK_IF_STATE, n, 0,
					 0, 0, &n);
		left -= n;
		ngroups++;
		if (fmode == MCAST_INCLUDE) 
        {
			group->ig_type = IGMP_REPORT_MODE_IN;
		    v3_mode_in_reports++;
		} 
        else 
        {
			group->ig_type = IGMP_REPORT_MODE_EX;
		    v3_mode_ex_reports++;
		}
		group->ig_datalen = 0;
		group->ig_numsrc = htons(n);
		group->ig_group = inm->inm_addr.s_addr;
        igmp_printk(KERN_DEBUG "igmp_send_if_report: type=%d  Group=0x%08x\n", 
                        group->ig_type, inm->inm_addr.s_addr);
		grouplen = IGMP_GRPREC_HDRLEN + IPV4SPACE(n);
		bytesavail -= grouplen;
		igmplen += grouplen;
		skb_put(skb, grouplen);

		/*
		 * If this membership has been reported, find next
		 */
		if (left <= 0) 
        {
			IN_NEXT_MULTI(step, inm);
			while (
                   inm && 
                   (
                    IS_IGMP_ALL_HOSTS(inm->inm_addr) || 
			        (ifp != inm->inm_ifp)
#ifdef CONFIG_RG_IGMP_PROXY
                    || (!igmprx_upstream_filter(ifp->dev->ifindex, inm->inm_addr.s_addr))
#endif /* CONFIG_RG_IGMP_PROXY */
#ifdef CONFIG_RG_IGMP_PROXY_MODULE
                     || 
                     ( 
                      igmp_proxy_upstream_filter &&
                      (!(igmp_proxy_upstream_filter(ifp->dev->ifindex, inm->inm_addr.s_addr)))
                     )
#endif /* CONFIG_RG_IGMP_PROXY_MODULE */
                   )
                  ) 
            {
				IN_NEXT_MULTI(step, inm);
			}
			if (inm == NULL) 
            {
				done = 1;
				left = 0;
			} 
            else 
            {
				left = igmp_tag_state(inm, &fmode, &slist, 0,
						      IGMP_MASK_IF_STATE);
			}
		}

		/*
		 * Either transmit or advance the group pointer
		 */
		if (done || 
		    (bytesavail < (IGMP_GRPREC_HDRLEN + IPV4SPACE(left)))) 
        {
			xmit = 1;
		} 
        else 
        {
			group = (struct igmp_grouprec *) ((char *) group +
				grouplen);
		}

		/*
		 * Transmit the report
		 */
		if (xmit) 
        {
			if (ip_route_output(&rt, dst, 0, 0, dev->ifindex)) 
            {
				igmp_printk(KERN_DEBUG "igmp_send_if_report: "
				       "ip_route_output\n");
				return;
			}
			if (rt->rt_src == 0) 
            {
				ip_rt_put(rt);
				igmp_printk(KERN_DEBUG "igmp_send_if_report: "
				       "rt->rt_src == 0\n");
				return;
			}
			skb->dst = &rt->u.dst;

			/*
			 * Set igmp and ip headers and transmit
			 */
			report->ir_type = (u_char) IGMP_V3_MEMBERSHIP_REPORT;
			report->ir_rsv1 = 0;
			report->ir_numgrps = htons(ngroups);
			report->ir_rsv2 = 0;
			report->ir_cksum = 0;
			report->ir_cksum = ip_compute_csum((void *) report, 
							   igmplen);
			
			ip->version = 4;
			ip->ihl = (sizeof(struct iphdr) + 4) >> 2;
			ip->tos = 0;
			ip->frag_off = __constant_htons(IP_DF);
			ip->ttl = 1;
			ip->saddr = rt->rt_src;
			ip->daddr = dst;
			ip->protocol = IPPROTO_IGMP;
			ip->tot_len = htons(igmplen + sizeof(struct iphdr) + 4);
			ip_select_ident(ip, &rt->u.dst, NULL);
			/*
			 * Construct router alert option
			 */
			((u8 *) &ip[1])[0] = IPOPT_RA;
			((u8 *) &ip[1])[1] = 4;
			((u8 *) &ip[1])[2] = 0;
			((u8 *) &ip[1])[3] = 0; 
			ip_send_check(ip);

			/* 
			 * transmit
			 */
			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, 
				rt->u.dst.dev, output_maybe_reroute);

		    igmpstat.igps_snd_v3_mode_in_reports += v3_mode_in_reports;
		    igmpstat.igps_snd_v3_mode_ex_reports += v3_mode_ex_reports;
		    igmpstat.igps_snd_v3_reports++;
			igmpstat.igps_snd_reports++;
            igmp_printk(KERN_DEBUG "igmp_send_if_report: Sent IGMPv3 Report for %d groups successfully on dev=%d/%s\n", 
                        ngroups, dev->ifindex, dev->name);

			/*
			 * Reset to build new message
			 */
			bytesavail = igmplen = ngroups = xmit = 0;
		}
	} /* while (!done) */		
}

/*
 * Send out a current-state report, update retransmission state, and return
 * whether a new current-state report has to be scheduled.
 */
static void igmp_current_state_report(struct in_multi *inmp, int userec)
{
	struct in_sfentry *slist;
	int fmode, cnt;
	unsigned short type;

	igmp_printk(KERN_DEBUG "igmp_current_state_report\n");

	/*
 	 * Tag current state and count the number of sources to report, and
	 * determine the mode to report which might be different from the
	 * membership mode in case of a group-and-source-specific query, which
	 * is always reported as IS_IN
	 */	
	cnt = igmp_tag_state(inmp, &fmode, &slist, userec, IGMP_MASK_CUR_STATE);
	
	/*
	 * Send report if it isn't an empty response to a source-specific query
	 * or an empty include (is_in) report
	 */
	if (!(cnt == 0 && (userec || fmode == MCAST_INCLUDE))) {
		if (fmode == MCAST_EXCLUDE) {
			type = IGMP_REPORT_MODE_EX;
		} else {
			type = IGMP_REPORT_MODE_IN;
		}
		igmp_send_report(inmp, slist, cnt, IGMP_MASK_CUR_STATE,type,0);
	}

	/*
	 * Clean recorded sources if they were used
	 */
	if (userec) {
		igmp_clean_recorded_sources(inmp);
	}
}

/*
 * Send out a source-list-change report.  All the source-list-changes in the
 * group 'inmp' are reported, both allow_new and block_old.
 */
static void igmp_send_slc_report(struct in_multi *inm, unsigned long addr)
{
    int v3_allow_reports=0, v3_block_reports=0;
	struct sk_buff *skb;
	struct iphdr *ip;
	struct rtable *rt;
	struct igmp_report *report;
	struct igmp_grouprec *group;
	struct in_sfentry *slist;
	int n, cnt, igmplen, nbytes, inex, tothlen, ngroups;
	int allowdone, blockdone;
	u32 dst;
	struct net_device *dev;

	igmp_printk(KERN_DEBUG "igmp_send_slc_report\n");

    dst = (addr != 0) ? addr : htonl(INADDR_ALLRTRS_IGMPV3_GROUP);
	dev = inm->inm_ifp->dev;

#ifdef CONFIG_RG_IGMP_PROXY
    if (!igmprx_upstream_filter(dev->ifindex, inm->inm_addr.s_addr))
        return;
#endif /* CONFIG_RG_IGMP_PROXY */
#ifdef CONFIG_RG_IGMP_PROXY_MODULE
    if (igmp_proxy_upstream_filter)
    {
        if (!(igmp_proxy_upstream_filter(dev->ifindex, inm->inm_addr.s_addr)))
            return;
    }
#endif /* CONFIG_RG_IGMP_PROXY_MODULE */

	tothlen = sizeof(struct iphdr) + 4 + IGMP_HDRLEN; /* 4 is for RA opt */

	/*
 	 * Send out the report, and, if necessary, use multiple datagrams.
	 */
	allowdone = blockdone = inex = 0;
	slist = inm->inm_sflincl;	

	do 
    {
		if (ip_route_output(&rt, dst, 0, 0, dev->ifindex)) 
        {
		   igmp_printk(KERN_DEBUG "igmp_send_slc_report: ip_route_output\n");
		   return;
		}
		if (rt->rt_src == 0) 
        {
		   ip_rt_put(rt);
		   igmp_printk(KERN_DEBUG "igmp_send_slc_report: rt->rt_src == 0\n");
		   return;
		} 

		/*
		 * Allocate mtu-sized skb 
		 */
		skb = alloc_skb(dev->mtu + dev->hard_header_len+15, GFP_ATOMIC);
		if (skb == NULL) 
        {
			ip_rt_put(rt);
			igmp_printk(KERN_DEBUG "igmp_send_slc_report: NULL skb\n");
			return;	
		}
		skb->dst = &rt->u.dst;
		skb_reserve(skb, (dev->hard_header_len + 15) & ~15);
		skb->nh.iph = ip = (struct iphdr *) skb_put(skb, 
			sizeof(struct iphdr) + 4);
		report = (struct igmp_report *) skb_put(skb, IGMP_HDRLEN);
		group = &(report->ir_groups[0]);
		ngroups = 0;
		igmplen = nbytes = dev->mtu - tothlen;

		/*
		 * If not done with allow_new, create an allow_new group record
		 */
		if (allowdone == 0) 
        {
			cnt = 0;
			nbytes -= IGMP_GRPREC_HDRLEN;
			n = nbytes / sizeof(struct in_addr);
			slist = igmp_get_sources(group, slist, 
				IGMP_MASK_ALLOW_NEW, n, 0, 1, 1, &n);
			nbytes -= (n * sizeof(struct in_addr));
			cnt += n;
			if (slist == NULL && inex == 0) 
            {
				slist = inm->inm_sflexcl;
				inex = 1;
				if (nbytes > sizeof(struct in_addr)) 
                {
					n = nbytes / sizeof(struct in_addr);
					slist = igmp_get_sources(group, slist,
						IGMP_MASK_ALLOW_NEW, n, cnt, 1,
						1, &n);
					nbytes -= (n * sizeof(struct in_addr));
					cnt += n;
				}
			}

			/*
			 * If done reporting allow_new, reset for block_old 
			 */
			if (slist == NULL) 
            {
				slist = inm->inm_sflincl;
				inex = 0;
				allowdone = 1;
			}

			/*
			 * Set the allow_new group record header
			 */
			if (cnt > 0) 
            {
				group->ig_type = (u_char) IGMP_REPORT_ALLOW_NEW;
                v3_allow_reports++;
				group->ig_datalen = 0;
				group->ig_numsrc = htons(cnt);
				group->ig_group = inm->inm_addr.s_addr;
				/* Set group to the next group position */
				group = (struct igmp_grouprec *) ((char *) group
					+ IGMP_GRPREC_HDRLEN + IPV4SPACE(cnt));
				ngroups++;
			} 
            else 
            {
				nbytes += IGMP_GRPREC_HDRLEN;
			}
		}	

		/*
		 * If bytes are left for something to report, report whatever
		 * needs to be reported as block_old
		 */
		if (nbytes > IGMP_GRPREC_HDRLEN + sizeof(struct in_addr)) 
        {
			cnt = 0;
			nbytes -= IGMP_GRPREC_HDRLEN;
			n = nbytes / sizeof(struct in_addr);
			slist = igmp_get_sources(group, slist,
				IGMP_MASK_BLOCK_OLD, n, 0, 1, 1, &n);
			nbytes -= (n * sizeof(struct in_addr));
			cnt += n;
			if (slist == NULL && inex == 0) {
				slist = inm->inm_sflexcl;
				inex = 1;
				if (nbytes > sizeof(struct in_addr)) {
					n = nbytes / sizeof(struct in_addr);
					slist = igmp_get_sources(group, slist,
						IGMP_MASK_BLOCK_OLD, n, cnt, 1,
						1, &n);
					nbytes -= (n * sizeof(struct in_addr));
					cnt += n;
				}
			}

			/*
			 * Done?
			 */
			if (slist == NULL) 
            {
				blockdone = 1;
			}

			/*
			 * Set the block_old group record header
			 */
			if (cnt > 0) 
            {
				group->ig_type = (u_char) IGMP_REPORT_BLOCK_OLD;
                v3_block_reports++;
				group->ig_datalen = 0;
				group->ig_numsrc = htons(cnt);
				group->ig_group = inm->inm_addr.s_addr;
				ngroups++;
			} 
            else 
            {
				nbytes += IGMP_GRPREC_HDRLEN;
			}
		}	

		skb_put(skb, igmplen - nbytes);
		igmplen = igmplen - nbytes + IGMP_HDRLEN;
		if (ngroups == 0) 
        {
			kfree_skb(skb);
			ip_rt_put(rt);
			return;
		}

		/*
		 * Set the report header
		 */
		report->ir_type = (u_char) IGMP_V3_MEMBERSHIP_REPORT;
		report->ir_rsv1 = 0;
		report->ir_numgrps = htons(ngroups);
		report->ir_rsv2 = htons(0);
		report->ir_cksum = 0;
		report->ir_cksum = ip_compute_csum((void *)report, igmplen);

		/*
		 * Set the ip header
		 */
		ip->version = 4;
		ip->ihl = (sizeof(struct iphdr) + 4) >> 2;
		ip->tos = 0;
		ip->frag_off = __constant_htons(IP_DF);
		ip->ttl = 1;
		ip->saddr = rt->rt_src;
		ip->daddr = dst;
		ip->protocol = IPPROTO_IGMP;
		ip->tot_len = htons(igmplen + sizeof(struct iphdr) + 4);  	
		ip_select_ident(ip, &rt->u.dst, NULL);
		/*
		 * Construct router alert option
		 */	
		((u8 *) &ip[1])[0] = IPOPT_RA;
		((u8 *) &ip[1])[1] = 4;
		((u8 *) &ip[1])[2] = 0;
		((u8 *) &ip[1])[3] = 0;
		ip_send_check(ip);	

		/*
		 * Send the report
		 */	
		NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, rt->u.dst.dev,
		        output_maybe_reroute);

        igmpstat.igps_snd_v3_allow_reports += v3_allow_reports;
        igmpstat.igps_snd_v3_block_reports += v3_block_reports;
		++igmpstat.igps_snd_v3_reports;
		++igmpstat.igps_snd_reports;
        igmp_printk(KERN_DEBUG "igmp_send_slc_report: Sent IGMPv3 SLC Report successfully for Group=0x%08x\n", 
                    inm->inm_addr.s_addr);


	} while (blockdone == 0);
}

/*
 * Send out a state-change report, which can be a filtermode-change or a
 * sourcelist-change, update retransmission state and return whether a new
 * state-change should be scheduled & if the membership record should be
 * deleted (through 'done')
 */
static int igmp_state_change_report(struct in_multi *inmp, int *done)
{
	struct in_sfentry *slist, *isf;
	unsigned short mask;
	unsigned char type;
	int fmode, cnt = 0;

	igmp_printk(KERN_DEBUG "igmp_state_change_report: 0x%08x(inm_rpt_toxx=%d / inm_rpt_statechg=%d)\n", 
                    inmp->inm_addr.s_addr, inmp->inm_rpt_toxx, inmp->inm_rpt_statechg);
	
	*done = 0;
	
	/*
 	 * If there is retransmission state for a filter-mode-change, report
	 * that first, a source-list-change otherwise.
	 */
	if (inmp->inm_rpt_toxx > 0) 
    {
	    igmp_printk(KERN_DEBUG "igmp_state_change_report: Send FilterModeChange Report for 0x%08x\n", 
                        inmp->inm_addr.s_addr);
		/*
		 * Tag the sources to be reported in this fmode-change report,
		 * unless it's the initialized state (include mode, with 0
		 * sources)
		 */
		if (inmp->inm_fmode == MCAST_INCLUDE) 
        {
			mask = IGMP_MASK_TO_IN;
		} 
        else 
        {
			mask = IGMP_MASK_TO_EX;
		}
		if (inmp->inm_fmode==MCAST_INCLUDE && inmp->inm_sflincl==NULL) 
        {
			cnt = 0;
			fmode = MCAST_INCLUDE;
			slist = NULL;
		} 
        else 
        {
			cnt = igmp_tag_state(inmp, &fmode, &slist, 0, mask);
		}

		/*
		 * Send out the report, which will decrement per timer 
		 * retransmission counter too if it has any (pending allow_new
		 * or block_old)
		 */
		if (inmp->inm_fmode == MCAST_EXCLUDE) 
        {
			type = IGMP_REPORT_TO_EX;
		} 
        else 
        {
			type = IGMP_REPORT_TO_IN;
		}
		igmp_send_report(inmp, slist, cnt, mask, type, 0);

		/*
		 * Update retransmission state and if this is the last filter-
		 * mode change report, see if membership is to be destroyed.
		 */
		if (inmp->inm_rpt_statechg > 0) 
			--inmp->inm_rpt_statechg;

		if (inmp->inm_rpt_toxx > 0) 
			--inmp->inm_rpt_toxx;

		if (inmp->inm_rpt_toxx == 0) 
		{
            if ((cnt == 0) && (inmp->inm_fmode == MCAST_INCLUDE))
			{
				/* Force to stop sending any more State Change reports */
				inmp->inm_rpt_statechg = 0;
				*done = 1;
			}
			else
			{
#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
				if (inmp->inm_startup_statechg_report)
				{
					inmp->inm_rpt_toxx = inmp->inm_rti->rti_qrv;
					inmp->inm_rpt_statechg = inmp->inm_rti->rti_qrv;
					inmp->inm_ti_statechg_interval = igmp_persistent_join_interval;
				}
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */
			}
		}
	} 
    else if (inmp->inm_rpt_statechg > 0) 
    {
	    igmp_printk(KERN_DEBUG "igmp_state_change_report: Send SourceListChange Report for 0x%08x\n", 
                        inmp->inm_addr.s_addr);
		/*
		 * Send out source-list changes.  This will decrease the
		 * retransmit timer.
		 */
		igmp_send_slc_report(inmp, 0);
		
		/*
		 * Decrease state-change-report counter
		 */
		if (inmp->inm_rpt_statechg > 0) 
			--inmp->inm_rpt_statechg;

		if (inmp->inm_rpt_statechg == 0) 
        {
#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
            if (inmp->inm_startup_statechg_report)
            {
                inmp->inm_rpt_toxx = inmp->inm_rti->rti_qrv;
                inmp->inm_rpt_statechg = inmp->inm_rti->rti_qrv;
                inmp->inm_ti_statechg_interval = igmp_persistent_join_interval;
            }
            else
            {
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */
                /*
                 * Done with source-list-changes, clear all remaining
                 * bits.
                 */
                SF_CLEARTAG(inmp->inm_sflincl, isf, IGMP_MASK_PENDING);
                SF_CLEARTAG(inmp->inm_sflexcl, isf, IGMP_MASK_PENDING);
                igmp_sf_unlink_unreferenced(inmp, MCAST_INCLUDE);
                igmp_sf_unlink_unreferenced(inmp, MCAST_EXCLUDE);

                /*
                 * If from INC{A} to INC{}, membership needs to be
                 * destroyed.
                 */
                if (inmp->inm_fmode == MCAST_INCLUDE) 
                {
                    SF_ISEMPTY(inmp->inm_sflincl, isf, *done);
                }
#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
            }
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */
		}
	}

	return (inmp->inm_rpt_statechg > 0);
}

/* 
 * Cancel and clear all pending reports
 */
static void igmp_cancel_all_reports(int cancel_statechg_too)
{
	struct router_info *rti;
	struct in_multistep step;
	struct in_multi *inmp;
	struct in_sfentry *isf;

    igmp_printk(KERN_DEBUG "igmp_cancel_all_reports\n");

	igmp_ft_dirty = igmp_ft_count = igmp_ft_stamp = 0;

	for (rti = Head; rti != NULL; rti = rti->rti_next) 
    {
		rti->rti_timer = 0;
	}

	IN_FIRST_MULTI(step, inmp);
	while (inmp != NULL) 
    {
		inmp->inm_timer = 0;
		inmp->inm_ti_curstate = 0;
		inmp->inm_state = 0;
		if (cancel_statechg_too) 
        {
			inmp->inm_ti_statechg = 0;
			/*
			 * Clear all tags that indicate pending report
			 */
			SF_CLEARTAG(inmp->inm_sflincl, isf, IGMP_MASK_PENDING);
			SF_CLEARTAG(inmp->inm_sflexcl, isf, IGMP_MASK_PENDING);
		}
		IN_NEXT_MULTI(step, inmp);
	}
}

/*
 * Fast timer: Check and send scheduled reports.
 */
static void igmp_fasttimo(unsigned long data)
{
	struct in_multi *inm, *inmdone = NULL;
	struct in_multistep step;
	struct router_info *rti;
	int done, delta, min, dirty;

	/*
	 * Quick check to see if any work needs to be done, in order to
	 * minimize the overhead of fisttimo processing.
	 */
	dirty = igmp_ft_dirty;
	igmp_ft_dirty = 0;
	if (((igmp_ft_count == 0) || --igmp_ft_count) && dirty == 0) 
    {
        mod_timer(&igmp_fasttimer, jiffies + IGMP_FASTTIMER_INTERVAL);
		return;
	}

	igmp_lock();
	delta = igmp_ft_stamp - igmp_ft_count;
	min = INT_MAX;

	/*
	 * Check all membership records to see if something needs to be 
	 * reported.
	 */
	IN_FIRST_MULTI(step, inm);
	while (inm != NULL) 
    {
		done = 0;
		/*
	  	 * No reports for all hosts
		 */
		if (inm->inm_addr.s_addr != IGMP_ALL_HOSTS) 
        {
			/*
			 * Handle current-state (solicited) reports
			 */
			if (inm->inm_timer != 0) 
            {
				if ((inm->inm_timer -= delta) <= 0) 
                {
				    inm->inm_timer = 0;
				    if (inm->inm_rti->rti_type == IGMP_V3_ROUTER) 
                    {
					    igmp_current_state_report(inm, 0);
				    } 
                    else 
                    {
					    igmp_sendpkt(inm,inm->inm_rti->rti_type, 0);
				    }
				    inm->inm_state |= IGMP_IREPORTEDLAST;
				} 
                else if (inm->inm_timer < min) 
                {
				    min = inm->inm_timer;
				}
			}

			/*
		 	 * Handle group-and-source-specific (solicited) reports
			 */
			if (inm->inm_ti_curstate != 0) 
            {
				if ((inm->inm_ti_curstate -= delta) <= 0) 
                {
					inm->inm_ti_curstate = 0;
					igmp_current_state_report(inm, 1);
				} 
                else if (inm->inm_ti_curstate < min) 
                {
					min = inm->inm_ti_curstate;
				}
			}

			/*
			 * Handle state-change reports
			 */
			if (inm->inm_ti_statechg != 0) 
            {
				if ((inm->inm_ti_statechg -= delta) <= 0) 
                {
				    inm->inm_ti_statechg = 0;
				    if (igmp_state_change_report(inm, &done)) 
                    {
						int state_change_interval;
						if (inm->inm_ti_statechg_interval == igmp_persistent_join_interval)
							state_change_interval = igmp_persistent_join_interval * PR_FASTHZ;
						else
							state_change_interval = IGMP_RANDOM_DELAY(igmp_host_unsol_interval * PR_FASTHZ);

						ASSIGN_FAST_TIMER(inm->inm_ti_statechg, state_change_interval);
                        igmp_printk(KERN_DEBUG "igmp_fasttimo: Scheduling FilterModeChange Report\n");

                        if (inm->inm_ti_statechg < min) 
                        {
                            min = inm->inm_ti_statechg;
                        }
				    } 
                    else if (done) 
                    {
					    inmdone = inm;
				    }
				} 
                else if (inm->inm_ti_statechg < min) 
                {
					min = inm->inm_ti_statechg;
				}
			}
		}

		/*
		 * Next membership record and delete current if 
		 * finished.
		 */
		IN_NEXT_MULTI(step, inm);
		if (done) 
        {
            igmp_printk(KERN_DEBUG "igmp_fasttimo: Deleting inm=%p\n", inm);
            /* Only delete inm if reference count drops to zero */
            if (inmdone->users <= 0) 
            {
                igmp_printk(KERN_DEBUG "igmp_fasttimo: Deleting inm=%p   users=0\n", inm);
                /* Remove from global multicast list */
                LIST_REMOVE(inmdone, inm_link);

                /* Remove from interface multicast list */
                igmp_unlink_intf_inm(inmdone);

                in_removemulti(inmdone);
            }
		}
	}
		
	/*
	 * Check interface timers
	 */
	for (rti = Head; rti != NULL; rti = rti->rti_next) 
    {
		if (rti->rti_timer != 0) 
        {
			if ((rti->rti_timer -= delta) <= 0) 
            {
				rti->rti_timer = 0;
				igmp_send_if_report(rti->rti_ifp);
			} 
            else if (rti->rti_timer < min) 
            {
				min = rti->rti_timer;
			}
		}
	}

	igmp_ft_count = igmp_ft_stamp = (min != INT_MAX) ? min : 0;

	mod_timer(&igmp_fasttimer, jiffies + IGMP_FASTTIMER_INTERVAL);
	igmp_unlock();
}

/*
 * Slow timer: Check router compatibility modes
 */
static void igmp_slowtimo(unsigned long data)
{
	struct router_info *rti = Head;
	int new_mode;

	igmp_lock();
	while (rti) 
    {
		new_mode = 0;

		/*
		 * Decrease router comp. mode timers and determine new mode if
		 * a timer expired
		 */
		if (rti->rti_timev2 != 0 && --rti->rti_timev2 == 1) 
        {
			new_mode = IGMP_V3_ROUTER;
		}
		if (rti->rti_timev1 != 0 && --rti->rti_timev1 == 1) 
        {
			if (rti->rti_timev2 > 1) 
            {
				new_mode = IGMP_V2_ROUTER;
			} 
            else 
            {
				new_mode = IGMP_V3_ROUTER;
			}
		}

		/*
		 * Switch
		 */
		if (new_mode != 0 && rti->rti_type != new_mode) 
        {
			rti->rti_type = new_mode;
			igmp_cancel_all_reports(1);
		}
		
		rti = rti->rti_next;
	}

	mod_timer(&igmp_slowtimer, jiffies + IGMP_SLOWTIMER_INTERVAL);
	igmp_unlock();
}

/*
 * Handle an incoming igmp message.
 */
int igmp_rcv(struct sk_buff *skb)
{
	struct iphdr *iph = skb->nh.iph;
	struct igmphdr *ih = skb->h.igmph;
	struct in_device *in_dev = in_dev_get(skb->dev);
	int len = skb->len;

	if (in_dev == NULL) 
    {
		kfree_skb(skb);
		return 0;
	}

	if (skb_is_nonlinear(skb)) 
    {
		if (skb_linearize(skb, GFP_ATOMIC) != 0) 
        {
		    in_dev_put(in_dev);
			kfree_skb(skb);
			return -ENOMEM;
		}
		ih = skb->h.igmph;
	}

	igmpstat.igps_rcv_total++;	/* XXX should this be placed earlier? */

	if (len < sizeof(struct igmphdr)) 
    {
		igmpstat.igps_rcv_tooshort++;
		in_dev_put(in_dev);
		kfree_skb(skb);
		return 0;
	}

	/*
	 * igmp messages should fit in one ip fragment (mtu)
	 */
	if (len > in_dev->dev->mtu) 
    {
		igmpstat.igps_rcv_toolong++;
		in_dev_put(in_dev);
		kfree_skb(skb);
		return 0;
	}
	
	/*
	 * Validate checksum
	 */	
	if (ip_compute_csum((void *)ih, len)) 
    {
		igmpstat.igps_rcv_badsum++;
		in_dev_put(in_dev);
		kfree_skb(skb);
		return 0;
	}

#ifdef CONFIG_RG_IGMP_PROXY
	igmprx_recv(skb);
#endif /* CONFIG_RG_IGMP_PROXY */
#ifdef CONFIG_RG_IGMP_PROXY_MODULE
	if (igmp_proxy_recv)
	    igmp_proxy_recv(skb);
#endif /* CONFIG_RG_IGMP_PROXY_MODULE */
	
    igmp_lock();

	switch (ih->type) 
    {
	    case IGMP_MEMBERSHIP_QUERY:
        {
            int mrt;
	        struct router_info *rti;
	        struct igmpv3 *igmp = (struct igmpv3 *) skb->h.igmph;

	        mrt = (ih->code) ? ih->code : 1;

            rti = get_rti(in_dev);
            if (!rti)
            {
                igmp_printk(KERN_DEBUG "igmp_rcv: Could not get rti\n");
                break;
            }

            /*
             * The query can be a v1, v2 or v3 query.
             */
            
            igmpstat.igps_rcv_queries++;

            if (ih->code == 0) 
            {
                /* Version 1 query */
                igmp_printk(KERN_DEBUG "igmp_rcv: received v1 query\n");

                mrt = IGMP_MAX_HOST_REPORT_DELAY * IGMP_TIMER_SCALE;
                if (iph->daddr != IGMP_ALL_HOSTS || ih->group != 0) 
                {
                    igmpstat.igps_rcv_badqueries++;
                    put_rti(in_dev);
                    break;
                }

                /* Set router variables */
                rti->rti_qrv = IGMP_INIT_ROBVAR;
                rti->rti_timev1 = rti->rti_qrv * IGMP_INIT_QRYINT;
                rti->rti_timev1 = (rti->rti_timev1+IGMP_DEF_QRYMRT) * PR_SLOWHZ;
            } 
            else if (len == 8) 
            {
                /* Version 2 query */
                igmp_printk(KERN_DEBUG "igmp_rcv: received v2 query\n");

                if (ih->group != 0 && !MULTICAST(ih->group)) 
                {
                    igmpstat.igps_rcv_badqueries++;
                    put_rti(in_dev);
                    break;
                }

                /* Set router variables */
                rti->rti_qrv = IGMP_INIT_ROBVAR;
                rti->rti_timev2 = rti->rti_qrv * IGMP_INIT_QRYINT;
                rti->rti_timev2 += ih->code / IGMP_TIMER_SCALE;
                rti->rti_timev2 *= PR_SLOWHZ;
            } 
            else if (len >= 12) 
            {
                /* Version 3 query */
                igmp_printk(KERN_DEBUG "igmp_rcv: received v3 query\n");

                /*
                 * Router alert option is required only for IGMPv3
                 * queries.
                 */
                if (!check_router_alert(iph)) 
                {
                    igmpstat.igps_rcv_badqueries++;
                    put_rti(in_dev);
                    break;
                }

                /*
                 * Drop when address or number of sources cannot be 
                 * valid.
                 */
                if ((igmp->igmp_group && !MULTICAST(igmp->igmp_group)) 
                    || ntohs(igmp->igmp_numsrc) > IGMP_MAXSOURCES(len)) 
                {
                    igmpstat.igps_rcv_badqueries++;
                    put_rti(in_dev);
                    break;
                }

                /* Set router variables */
                rti->rti_qrv = IGMP_QRV(igmp);
            } 
            else 
            {
                /*
                 * Invalid length -- discard.
                 */
                igmpstat.igps_rcv_badqueries++;
                put_rti(in_dev);
                break;
            }	

            /*
             * Check whether host compatibility mode should be changed.  If
             * so, cancel pending responses and retransmission timers.
             */
            if (rti->rti_timev1 <= 0) 
            {
                if (rti->rti_timev2 <= 0 && 
                                rti->rti_type != IGMP_V3_ROUTER) 
                {
                    rti->rti_type = IGMP_V3_ROUTER;
                    igmp_cancel_all_reports(1);
                } 
                else if (rti->rti_timev2 > 0 &&
                         rti->rti_type != IGMP_V2_ROUTER) 
                {
                    rti->rti_type = IGMP_V2_ROUTER;
                    igmp_cancel_all_reports(1);
                }
            } 
            else if (rti->rti_timev1 > 0) 
            { 
                if (rti->rti_type != IGMP_V1_ROUTER) 
                {
                    rti->rti_type = IGMP_V1_ROUTER;
                    igmp_cancel_all_reports(1);
                }
            }

            /* Schedule the response(s) to this query */
            igmp_schedule_curstate_reports(in_dev,rti,igmp,len,mrt,skb);
            put_rti(in_dev);
            break;
        }

	    case IGMP_V1_MEMBERSHIP_REPORT:
	    case IGMP_V2_MEMBERSHIP_REPORT:
        {
	        int self;
	        struct in_addr in;
	        struct in_multi *inm=NULL;

            if (ih->type == IGMP_V1_MEMBERSHIP_REPORT) 
                igmp_printk(KERN_DEBUG "igmp_rcv: received v1 report\n");
            else
                igmp_printk(KERN_DEBUG "igmp_rcv: received v2 report\n");

            /*
             * In the IGMPv2 specification, there are 3 states and a flag.
             * In Non-Member state, we simply don't have a membership
             * record. In Delaying Member state, our timer is running
             * (inm->inm_timer != 0).  In Idle Member state, our timer is
             * not running (inm->inm_timer == 0).
             *
             * The flag is inm->inm_state.  It is set to IGMP_OTHERMEMBER
             * if we have last heard a report from another member, or to
             * IGMP_IREPORTEDLAST if we were the last to send a report.
             */
            
            /*
             * For fast leave to work, we have to know that we are the last
             * person to send a report for this group. Reports can be
             * looped back if we are a multicast router, so we discard 
             * reports for which we are the source.   
             */
            self = 0;
            for_ifa(in_dev) 
            {
                igmp_printk(KERN_DEBUG "igmp_rcv: ifa->ifa_address is %u.%u.%u.%u\n",
                            NIPQUAD(ifa->ifa_address));
                igmp_printk(KERN_DEBUG "igmp_rcv: iph->saddr is %u.%u.%u.%u\n",
                            NIPQUAD(iph->saddr));
                if (ifa->ifa_address == iph->saddr) 
                {
                    igmp_printk(KERN_DEBUG "igmp_rcv: ignoring own report\n");
                    self = 1;
                    break;
                }		
            } endfor_ifa(in_dev);

            if (self) 
                break;

            igmpstat.igps_rcv_v2_reports++;

            if (in_dev->dev->flags & IFF_LOOPBACK) 
            {
                igmp_printk(KERN_DEBUG "igmp_rcv: discarding loopback report\n");
                break;
            }

            if (!MULTICAST(ih->group)) 
            {
                igmpstat.igps_rcv_v2_badreports++;
                break;
            }		

            /* 
             * The BSD code rewrites the IP source address of those reports
             * with an unspecified (i.e., zero) subnet number so that
             * process level daemons can determine which subnet such a 
             * report arrived from.  That wouldn't help here, since 
             * igmp_rcv() is not responsible for passing igmp msgs to
             * process level. 
             *
             * XXX Ought we to do this elsewhere, though?
             */

            /*
             * If we belong to the group being reported, stop our timer for
             * that group.
             */
            in.s_addr = ih->group;
            read_lock(&in_dev->lock);
            IN_LOOKUP_MULTI(in, in_dev, inm);
            if (inm != NULL) 
            {
                inm->inm_timer = 0;
                igmpstat.igps_rcv_v2_ourreports++;
                inm->inm_state |= IGMP_OTHERMEMBER;	
            }
            read_unlock(&in_dev->lock);
            break;
        }

	    case IGMP_PIM:
        {
            igmp_printk(KERN_DEBUG "igmp_rcv: received pim msg\n");
#ifdef CONFIG_IP_PIMSM_V1
            igmp_unlock();
            in_dev_put(in_dev);
            return pim_rcv_v1(skb);
#endif
        }

	    case IGMP_V2_LEAVE_GROUP:
        {
            igmpstat.igps_rcv_v2_leaves++;
	    	break;
        }

	    case IGMP_V3_MEMBERSHIP_REPORT:
        {
            igmpstat.igps_rcv_v3_reports++;
            break;
        }
 
	    case IGMP_DVMRP:
	    case IGMP_TRACE:
	    case IGMP_MTRACE_RESP:
	    case IGMP_MTRACE:
        {
            /*
             * We do nothing on receipt of these other known message types
             */
            igmp_printk(KERN_DEBUG "igmp_rcv: received other msg\n");
	    	break;
        }

	    default:
        {
		    NETDEBUG(printk(KERN_DEBUG "Unknown IGMP type=%d\n", ih->type));
        }
	}

    igmp_unlock();

	/*
	 * Note: Unlike BSD code, Linux's igmp_rcv() is not responsible for
	 * passing these messages up to processes listening on raw sockets.
	 * We can go ahead and release the packet's resources.
	 */
	in_dev_put(in_dev);
	kfree_skb(skb);
	return 0;
}

/*
 * Are we interested in receiving a packet, via igmp_rcv(), that arrived
 * on interface 'in_dev' with multicast destination address 'mc_addr'?
 */
int ip_check_mc(struct in_device *in_dev, u32 mc_addr)
{
	struct in_multi *inm;

	igmp_printk(KERN_DEBUG "ip_check_mc: dev=%s    mc_addr=0x%08x\n",
                    in_dev->dev->name, mc_addr);
	igmp_lock();

    read_lock(&in_dev->lock); 
	for (inm = in_dev->mc_list; inm; inm = inm->inm_next) 
    {
		if (inm->inm_addr.s_addr == mc_addr) 
        {
            read_unlock(&in_dev->lock);
			igmp_unlock();
            return 1;
        }
    }
    read_unlock(&in_dev->lock);

	igmp_unlock();
    return 0;
}

/*
 * Initialize igmp.
 */
void igmp_init(void)
{
    igmp_proc_dir = proc_mkdir("net/igmp", NULL);
    
    create_proc_read_entry("host", 0, igmp_proc_dir, ip_mc_procinfo, NULL);
    create_proc_read_entry("stats", 0, igmp_proc_dir, ip_mc_stats, NULL);

	/*
	 * Fast timer processing
	 */
	igmp_ft_count = igmp_ft_dirty = igmp_ft_stamp = 0;

    /* Intialize the global multicast list HEAD */
    in_multihead.lh_first = NULL;

	Head = (struct router_info *) 0;

	/*
 	 * Initialize timers, emulating BSD environment
	 */
	init_timer(&igmp_fasttimer);
	igmp_fasttimer.data = 0;	
	igmp_fasttimer.function = igmp_fasttimo;
	mod_timer(&igmp_fasttimer, jiffies + 1);

	init_timer(&igmp_slowtimer);
	igmp_slowtimer.data = 0;
	igmp_slowtimer.function = igmp_slowtimo;
	mod_timer(&igmp_slowtimer, jiffies + 1);

    printk(KERN_INFO "igmp_init: Iniatialized IGMP\n");
}

/*
 * Join a group by sending a membership report
 */
static void igmp_joingroup(struct in_multi *inm)
{
	igmp_printk(KERN_DEBUG "igmp_joingroup\n");

	ip_mc_filter_add(inm->inm_ifp, inm->inm_addr.s_addr);

	/*
	 * With IGMPv3, all membership records need router info.	
	 */
	inm->inm_rti = find_or_create_rti(inm->inm_ifp);
	if (inm->inm_rti == NULL) 
    {
		/*
		 * See comments in find_or_create_rti().  If the malloc fails, we
		 * log an error and bail.  Anything better to do?
		 */
		igmp_printk(KERN_DEBUG "igmp_joingroup: couldn't obtain rti\n");
		return;
	}

	if ((inm->inm_addr.s_addr == IGMP_ALL_HOSTS) ||
	    (inm->inm_ifp->dev->flags & IFF_LOOPBACK)) 
    {
		inm->inm_timer = 0;
		inm->inm_state |= IGMP_OTHERMEMBER;
	} 
    else 
    {
		/* 
		 * In IGMPv3 mode, the change will be through a state-change
		 */
		if (inm->inm_rti->rti_type != IGMP_V3_ROUTER) 
        {
			inm->inm_state |= IGMP_IREPORTEDLAST;
			inm->inm_rpt_statechg = inm->inm_rti->rti_qrv;
#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
			inm->inm_ti_statechg_interval = igmp_host_unsol_interval;
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */
			ASSIGN_FAST_TIMER(inm->inm_timer, 1);
		}
	}
}

/*
 * Leave a group. Send the leave message and clean up the membership record.
 */
static void igmp_leavegroup(struct in_multi *inmp)
{
	igmp_printk(KERN_DEBUG "igmp_leavegroup inmp=%p\n", inmp);

    if (inmp->loaded) 
    {
        inmp->loaded = 0;
        ip_mc_filter_del(inmp->inm_ifp, inmp->inm_addr.s_addr);
    }

	/*
	 * Send the 'right' leave message if this membership/interface requires
	 * reports.
	 */
	if (inmp->inm_addr.s_addr != IGMP_ALL_HOSTS && 
        !(inmp->inm_ifp->dev->flags & IFF_LOOPBACK) &&
	    inmp->inm_rti && inmp->inm_rti->rti_type != IGMP_V1_ROUTER) 
    {
		if (inmp->inm_rti->rti_type == IGMP_V3_ROUTER) 
        {
			/*
			 * Schedule leave reports. If current mode is include,
		   	 * the scheduled change will be ok (block old), but if
			 * it's exclude, a to_in{} report must be enforced.
			 * After final leave has been sent, the membership
			 * record will be deleted.
			 */
			inmp->inm_timer = inmp->inm_ti_curstate = inmp->inm_ti_statechg = 0;
			if (inmp->inm_fmode == MCAST_EXCLUDE) 
            {
				inmp->inm_sflincl = NULL;
				inmp->inm_sflexcl = NULL;
				inmp->inm_sflrec = NULL;
				inmp->inm_fmode = MCAST_INCLUDE;
				inmp->inm_rpt_toxx = inmp->inm_rti->rti_qrv;
			}
			inmp->inm_rpt_statechg = inmp->inm_rti->rti_qrv;
#ifdef ACTION_TEC_STB_PROTECTION
			inm->inm_ti_statechg_interval = igmp_host_unsol_interval;
#endif /* ACTION_TEC && CONN_WIZARD_DEBUG */
			ASSIGN_FAST_TIMER(inmp->inm_ti_statechg, 1);
	        igmp_printk(KERN_DEBUG "igmp_leavegroup inmp=%p Scheduled StateChange report\n", inmp);
		} 
        else if (inmp->inm_state & IGMP_IREPORTEDLAST) 
        {
			/*
			 * With a v2 router, send out a v2 leave report
			 */
			igmp_sendpkt(inmp,IGMP_V2_LEAVE_GROUP,IGMP_ALL_ROUTER);
	        igmp_printk(KERN_DEBUG "igmp_leavegroup inmp=%p Sent IGMPv2 Leave\n", inmp);
		}
	}		
}

/*
 *	Add a filter to a device
 */
static void ip_mc_filter_add(struct in_device *in_dev, u32 addr)
{
	char buf[MAX_ADDR_LEN];
	struct net_device *dev = in_dev->dev;

	igmp_printk(KERN_DEBUG "ip_mc_filter_add: %s\n", in_dev->dev->name);
	/* Checking for IFF_MULTICAST here is WRONG-WRONG-WRONG.
	   We will get multicast token leakage, when IFF_MULTICAST
	   is changed. This check should be done in dev->set_multicast_list
	   routine. Something sort of:
	   if (dev->mc_list && dev->flags&IFF_MULTICAST) { do it; }
	   --ANK
	   */
	if (arp_mc_map(addr, buf, dev, 0) == 0)
		dev_mc_add(dev,buf,dev->addr_len,0);
}

/*
 *	Remove a filter from a device
 */
static void ip_mc_filter_del(struct in_device *in_dev, u32 addr)
{
	char buf[MAX_ADDR_LEN];
	struct net_device *dev = in_dev->dev;

	igmp_printk(KERN_DEBUG "ip_mc_filter_del: %s\n", in_dev->dev->name);
	if (arp_mc_map(addr, buf, dev, 0) == 0)
		dev_mc_delete(dev,buf,dev->addr_len,0);
}

/*
 *	Multicast list managers
 */

/*
 *	A socket has joined a multicast group on device dev.
 */
void ip_mc_inc_group(struct in_device *in_dev, u32 addr)
{
	igmp_printk(KERN_DEBUG "ip_mc_inc_group: dev=%s  group=0x%08x\n", 
                in_dev->dev->name, addr);
	/* XXX -- do something for GRE module */
}

/*
 *	A socket has left a multicast group on device dev
 */
int ip_mc_dec_group(struct in_device *in_dev, u32 addr)
{
	igmp_printk(KERN_DEBUG "ip_mc_dec_group: dev=%s  group=0x%08x\n", 
                in_dev->dev->name, addr);
	/* XXX -- do something for GRE module */
	/* XXX -- do something for GRE module */
	return 0;
}

/* 
 * Device going down 
 */
void ip_mc_down(struct in_device *in_dev)
{
	igmp_printk(KERN_DEBUG "ip_mc_down: START dev=%s(%d)\n", 
                in_dev->dev->name, atomic_read(&(in_dev->dev->refcnt)));

	ASSERT_RTNL();

#if 0
	igmp_lock();

#ifdef CONFIG_RG_IGMP_PROXY
	igmprx_if_down(in_dev);
#endif /* CONFIG_RG_IGMP_PROXY */
#ifdef CONFIG_RG_IGMP_PROXY_MODULE
	if (igmp_proxy_if_down)
	    igmp_proxy_if_down(in_dev);
#endif /* CONFIG_RG_IGMP_PROXY_MODULE */

	/*
	 * If the interface supports multicast, leave the "all hosts" 
	 * multicast group on that interface
	 */

	if (in_dev->dev->flags & IFF_MULTICAST) 
    {
		struct in_multi *inm, *inmnext;

        inm = in_dev->mc_list;
        while (inm != NULL)
        {
            inmnext = inm->inm_next;

			if (inm->inm_addr.s_addr != IGMP_ALL_HOSTS) 
                in_cleanupmulti(inm);

            inm = inmnext;
        }
    }

	igmp_unlock();
#endif

	igmp_printk(KERN_DEBUG "ip_mc_down: END dev=%s(%d)\n", 
                in_dev->dev->name, atomic_read(&(in_dev->dev->refcnt)));
}

/* 
 * Device going up 
 */
void ip_mc_up(struct in_device *in_dev)
{
	igmp_printk(KERN_DEBUG "ip_mc_up: START dev=%s(%d)\n", 
                in_dev->dev->name, atomic_read(&(in_dev->dev->refcnt)));

	ASSERT_RTNL();

#if 0
	igmp_lock();

#ifdef CONFIG_RG_IGMP_PROXY
	igmprx_if_up(in_dev);
#endif /* CONFIG_RG_IGMP_PROXY */
#ifdef CONFIG_RG_IGMP_PROXY_MODULE
	if (igmp_proxy_if_up)
	    igmp_proxy_if_up(in_dev);
#endif /* CONFIG_RG_IGMP_PROXY_MODULE */

	igmp_unlock();
#endif

	igmp_printk(KERN_DEBUG "ip_mc_up: END dev=%s(%d)\n", 
                in_dev->dev->name, atomic_read(&(in_dev->dev->refcnt)));
}

/*
 *	Device is about to be destroyed: clean up.
 */
void ip_mc_destroy_dev(struct in_device *in_dev)
{
	printk(KERN_CRIT "ip_mc_destroy_dev: START dev=%s(%d)\n", 
                in_dev->dev->name, atomic_read(&(in_dev->dev->refcnt)));

	/*
	 * If the interface supports multicast, leave the "all hosts" 
	 * multicast group on that interface
	 */
	igmp_lock();

	if (in_dev->dev->flags & IFF_MULTICAST) 
    {
		struct in_multi *inm, *inmnext;

        inm = in_dev->mc_list;
        while (inm != NULL)
        {
            inmnext = inm->inm_next;

			if (inm->inm_addr.s_addr != IGMP_ALL_HOSTS) 
                in_cleanupmulti(inm);

            inm = inmnext;
        }
    }

	igmp_unlock();

	printk(KERN_CRIT "ip_mc_destroy_dev: END dev=%s(%d)\n", 
                in_dev->dev->name, atomic_read(&(in_dev->dev->refcnt)));
}

/*
 *	A socket is closing.
 */
void ip_mc_drop_socket(struct sock *sk)
{
	int i;
	struct source_list *psrc, *psrc2;
	struct inet_opt *inet = &sk->protinfo.af_inet;

	if (inet->mc_list == NULL)
		return;

	igmp_printk(KERN_DEBUG "ip_mc_drop_socket: START\n");
	rtnl_lock();
	igmp_lock();
	for (i = 0; i < sk->protinfo.af_inet.mc_num_memberships; i++) 
    {
		/* 
		 * Remove source filter list and tell igmp
		 */
		psrc = sk->protinfo.af_inet.mc_membershipsf[i].smsf_slist;
		sk->protinfo.af_inet.mc_membershipsf[i].smsf_slist = NULL;
		igmp_chg_sourcefilter(sk->protinfo.af_inet.mc_membership[i], 
		    psrc, sk->protinfo.af_inet.mc_membershipsf[i].smsf_fmode, 
                    NULL, MCAST_INCLUDE);	
		while (psrc) 
        {
			psrc2 = psrc->src_next;
			kfree(psrc);
			psrc = psrc2;
		}
		/*
		 * Tell the interface
		 */
		in_destroymulti(sk->protinfo.af_inet.mc_membership[i]);
	}
	igmp_unlock();
	rtnl_unlock();
	igmp_printk(KERN_DEBUG "ip_mc_drop_socket: END\n");
}

/*
 * Dynamically generate contents of /proc/net/igmp file. 
 */
static int ip_mc_procinfo(char *buffer, char **start, off_t offset, 
                          int length, int *eof, void *data)
{
	int len=0;
	off_t pos=0, begin=0;
	struct net_device *dev;

	len = sprintf(buffer + len, "\nUpstream IGMP Proxy Information\n");

	len += sprintf(buffer + len, "Client Unsolicited Report Interval=%d\n", igmp_host_unsol_interval);
#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
	len += sprintf(buffer + len, "Client Persistent Join Report Interval=%d\n", igmp_persistent_join_interval);
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */

	len += sprintf(buffer + len, "************************************************************************************\n");
	len += sprintf(buffer + len , "Idx\tDevice\tVersion\tQRV\tGroupCount\n");
	len += sprintf(buffer + len, "************************************************************************************\n");
	igmp_lock();
	read_lock(&dev_base_lock);
	for (dev = dev_base; dev; dev = dev->next) 
    {
		struct in_device *in_dev;
		struct router_info *rti;
		struct in_multi *inm;
			
		/* XXX  to shut up compiler */
		if (pos)
			goto done;

        if (!dev)
            goto done;

		in_dev = in_dev_get(dev);
		if (in_dev == NULL) 
			continue;

		/*
		 * Print basic device info
		 */
		len += sprintf(buffer + len, "%d\t%s\t", dev->ifindex, dev->name);
	
		read_lock(&in_dev->lock);	

		rti = get_rti(in_dev);

		/*
	 	 * Print per-interface info stored in router info struct
		 */
		if (rti) 
        {
			len += sprintf(buffer + len, "%s\t%d\t%d\n", 
				            rti_version_string(rti->rti_type), 
                            rti->rti_qrv, in_dev->mcgrp_count);

            put_rti(in_dev);
		} 
        else 
        {
			len += sprintf(buffer + len, "none\tnone\n");	
			read_unlock(&in_dev->lock);
			in_dev_put(in_dev);
			continue;
		}

		/*
		 * Walk interface's multicast subscription state
		 */
		len += sprintf(buffer + len, "\t================================================================================\n");
		len += sprintf(buffer + len, "\tGroup\t\tMode\tUsers\n");
		len += sprintf(buffer + len, "\tSrclist\n");
		len += sprintf(buffer + len, "\tLocal\tLastReport\tTotalJoins\tTotalLeaves\tPersisJoin\tinm_state\n");
		len += sprintf(buffer + len, "\tinm_timer\tinm_ti_curstate\tinm_ti_statechg\tinm_rpt_statechg\tinm_rpt_toxx\n");
		len += sprintf(buffer + len, "\t================================================================================\n");

        if (in_dev->mc_list)
        {
		    for (inm = in_dev->mc_list; inm; inm = inm->inm_next) 
            {
			    struct in_sfentry *sf;

			    len += sprintf(buffer + len, "\t%u.%u.%u.%u\t%s\t%d\n", 
                                NIPQUAD(inm->inm_addr.s_addr), 
                                ip_mc_fmode_string(inm->inm_fmode), 
                                inm->users);
                /*
                 * Walk appropriate source list
                 * XXX print elements with 0 refcount? 
                 */
			    len += sprintf(buffer + len, "\t{ ");
                SF_START(inm->inm_fmode, inm, sf);
                if (sf)
                {
                    for ( ; sf; sf = sf->isf_next) 
                    {
                        if (sf->isf_refcount != 0) 
                        {
                            len += sprintf(buffer + len, "%u.%u.%u.%u ", 
                                            NIPQUAD(sf->isf_addr.s_addr));
                        }
                    }	
                }
			    len += sprintf(buffer + len, "}\n");
			    len += sprintf(buffer + len, "\t%-5d\t%-10d\t%-10d\t%-11d\t%-10d\t0x%04x\n", 
                                inm->inm_grp_self,
                                inm->inm_last_report_time,
                                inm->inm_total_joins,
                                inm->inm_total_leaves,
                                inm->inm_startup_statechg_report,
								inm->inm_state);
			    len += sprintf(buffer + len, "\t%-9d\t%-15d\t%-7d/%-7d\t%-16d\t%-12d\n", 
                                inm->inm_timer,
                                inm->inm_ti_curstate,
                                inm->inm_ti_statechg,
                                inm->inm_ti_statechg_interval,
                                inm->inm_rpt_statechg,
                                inm->inm_rpt_toxx);
				if (inm->inm_next != NULL)
					len += sprintf(buffer + len, "\t--------------------------------------------------------------------------------\n");
		    }
        }
		len += sprintf(buffer + len, "\t================================================================================\n");

		read_unlock(&in_dev->lock);
		in_dev_put(in_dev);
		len += sprintf(buffer + len, "\n");
	}
	len += sprintf(buffer + len, "************************************************************************************\n");
done:
	read_unlock(&dev_base_lock);
	igmp_unlock();
	*start=buffer+(offset-begin);
    *eof = 1;
	len-=(offset-begin);
	if(len>length)
		len=length;
	if(len<0)
		len=0;
	return len;
}

/* IGMP Multicast Statistics for GUI */
static int ip_mc_stats(char *buffer, char **start, off_t offset, 
                       int length, int *eof, void *data)
{
    int len=0;

    len += sprintf(buffer+len, "************************************************************\n");
    len += sprintf(buffer+len, "IGMP Statistics\n");
    len += sprintf(buffer+len, "************************************************************\n");

    igmp_lock();
    len += sprintf(buffer+len, "%-40s: %10u\n", "Total IGMP messages received", 
                        igmpstat.igps_rcv_total);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMP received Too Short", 
                        igmpstat.igps_rcv_tooshort);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMP received Too Long", 
                        igmpstat.igps_rcv_toolong);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMP received with Bad Checksum", 
                        igmpstat.igps_rcv_badsum);

    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMP Valid Queries received", 
                        igmpstat.igps_rcv_queries);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMP Invalid Queries received", 
                        igmpstat.igps_rcv_badqueries);

    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMPv1/v2 Valid Reports/Joins received", 
                        igmpstat.igps_rcv_v2_reports);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMPv1/v2 Invalid Reports/Joins received", 
                        igmpstat.igps_rcv_v2_badreports);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMPv1/v2 Reports for known Groups", 
                        igmpstat.igps_rcv_v2_ourreports);

    len += sprintf(buffer+len, "%-40s: %10u\n", "Total IGMP Reports transmitted", 
                        igmpstat.igps_snd_reports);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMPv1 Joins transmitted", 
                        igmpstat.igps_snd_v1_joins);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMPv2 Joins transmitted", 
                        igmpstat.igps_snd_v2_joins);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMPv2 Leaves transmitted", 
                        igmpstat.igps_snd_v2_leaves);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMPv3 Reports transmitted", 
                        igmpstat.igps_snd_v3_reports);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMPv3 MODE_IN transmitted", 
                        igmpstat.igps_snd_v3_mode_in_reports);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMPv3 MODE_EX transmitted", 
                        igmpstat.igps_snd_v3_mode_ex_reports);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMPv3 TO_IN transmitted", 
                        igmpstat.igps_snd_v3_to_in_reports);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMPv3 TO_EX transmitted", 
                        igmpstat.igps_snd_v3_to_ex_reports);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMPv3 ALLOW transmitted", 
                        igmpstat.igps_snd_v3_allow_reports);
    len += sprintf(buffer+len, "%-40s: %10u\n", "IGMPv3 BLOCK transmitted", 
                        igmpstat.igps_snd_v3_block_reports);
    len += sprintf(buffer+len, "************************************************************\n\n");
    igmp_unlock();

    len -= offset;

    if (len > length)
        len = length;
    if (len < 0)
        len = 0;

    *start = buffer + offset;
    *eof = 1;

    return len;
}

/*
 * Add a membership record to a socket's multicast options
 */ 
static int ip_addmembership(struct sock *sk, struct in_addr *group, 
                     struct in_device *ifp, int *index)
{
	int i;

	igmp_printk(KERN_DEBUG "ip_addmembership\n");

	/*
 	 * If no interface address was provided, use the interface of the
	 * route to the given multicast address.
	 */
	if (ifp == NULL) 
    {
		struct rtable *rt;
		struct net_device *dev = NULL;

		if (!ip_route_output(&rt, group->s_addr, 0, 0, 0)) 
        {
			dev = rt->u.dst.dev;
			ip_rt_put(rt);
		}
		if (dev) 
        {
		    ifp = __in_dev_get(dev);
		}
    }

	/*
	 * See if we found an interface, and confirm that it supports 
	 * multicast.
	 */
	if (ifp == NULL || (ifp->dev->flags & IFF_MULTICAST) == 0)
		return -EADDRNOTAVAIL;
	

	/*
 	 * See if the membership already exists or if all the membership slots
	 * are full.
	 */
	for (i = 0; i < sk->protinfo.af_inet.mc_num_memberships; i++) {
	  	if ((sk->protinfo.af_inet.mc_membership[i]->inm_ifp == ifp) &&
	     	    (sk->protinfo.af_inet.mc_membership[i]->inm_addr.s_addr == group->s_addr))
			break;	
	}	
	if (i == sysctl_igmp_max_memberships)
		return -ETOOMANYREFS;
	*index = i;
	if (i < sk->protinfo.af_inet.mc_num_memberships)
		return -EADDRINUSE;

	/*
	 * Everything looks good; add a new record to the multicast address
	 * list for the given interface.
	 */
	if ((sk->protinfo.af_inet.mc_membership[i] = in_createmulti(group, ifp)) == NULL) 
		return -ENOBUFS;

    ++sk->protinfo.af_inet.mc_membership[i]->inm_grp_self;

	/*
	 * Set the multicast source filter of this entry (socket, group,
	 * interface) to an empty list in include mode.
	 */
	sk->protinfo.af_inet.mc_membershipsf[i].smsf_fmode = MCAST_INCLUDE;
	sk->protinfo.af_inet.mc_membershipsf[i].smsf_slist = NULL;
	sk->protinfo.af_inet.mc_membershipsf[i].smsf_numsrc = 0;

	sk->protinfo.af_inet.mc_num_memberships++;
   	return 0;	
}

/*
 * Find a membership record within a socket's multicast options
 */
static int ip_findmembership(struct sock *sk, struct in_addr *group, 
                      struct in_device *ifp, int *index)
{
	int i;

	/*
 	 * Find the membership in the membership array.
	 */
	for (i = 0; i < sk->protinfo.af_inet.mc_num_memberships; i++) {
		if ((ifp == NULL || 
		    sk->protinfo.af_inet.mc_membership[i]->inm_ifp == ifp) &&
		    sk->protinfo.af_inet.mc_membership[i]->inm_addr.s_addr ==
		    group->s_addr)
			break;
	}
	if (i == sk->protinfo.af_inet.mc_num_memberships)
		return -EADDRNOTAVAIL;

	*index = i;
	return 0;    
}

/*
 * Drop a membership record of a socket
 */
static int ip_dropmembership(struct sock *sk, struct in_addr *group, 
 		      struct in_device *ifp)
{
	struct source_list *psrc, *psrc2;
	int err, i;

	if ((err = ip_findmembership(sk, group, ifp, &i)) != 0) 
		return err;

	/*
	 * Remove socket's source filters
	 */
	psrc = sk->protinfo.af_inet.mc_membershipsf[i].smsf_slist;
	sk->protinfo.af_inet.mc_membershipsf[i].smsf_slist = NULL;
        while (psrc != NULL) {
		psrc2 = psrc->src_next;
		kfree(psrc);
		psrc = psrc2;
	}

	/*
	 * Give up the multicast address record to which the membership points.
	 */
    --sk->protinfo.af_inet.mc_membership[i]->inm_grp_self;
	in_destroymulti(sk->protinfo.af_inet.mc_membership[i]);
	
	/*
	 * Remove the gap in the membership and membershipsf arrays.
	 */
	for (i++; i < sk->protinfo.af_inet.mc_num_memberships; i++) {
		sk->protinfo.af_inet.mc_membership[i-1] = 
		    sk->protinfo.af_inet.mc_membership[i];
		sk->protinfo.af_inet.mc_membershipsf[i-1].smsf_fmode =
		    sk->protinfo.af_inet.mc_membershipsf[i].smsf_fmode;
		sk->protinfo.af_inet.mc_membershipsf[i-1].smsf_slist =
		    sk->protinfo.af_inet.mc_membershipsf[i].smsf_slist;
		sk->protinfo.af_inet.mc_membershipsf[i-1].smsf_numsrc =
		    sk->protinfo.af_inet.mc_membershipsf[i].smsf_numsrc;
	}
 	sk->protinfo.af_inet.mc_num_memberships--;
	return 0;	
}

/*
 * Get the multicast membership argument for a call to setsockopt()
 */
static int ip_get_sopt_mreq_arg(int optname, char *optval, int optlen, 
    struct in_addr *gaddr, struct in_addr *saddr, struct in_device **ifpp)
{
	int err = 0;

	igmp_printk(KERN_DEBUG "ip_get_sopt_mreq_arg\n");

	switch (optname) 
    {
	    
	    case IP_ADD_MEMBERSHIP:
	    case IP_DROP_MEMBERSHIP: 
        {
		/*
		 * Argument is 'struct ip_mreq'
		 * XXX support for ip_mreqn?
		 */
		struct ip_mreq mreq;

		/*
 		 * Copy mreq struct from userspace
		 */
		if (optlen < sizeof(struct ip_mreq))
			return -EINVAL;		
		if (copy_from_user(&mreq, optval, sizeof(struct ip_mreq)))
			return -EFAULT;

		/*
 		 * If interface specified, find it and hold a reference on it.
		 */
        if (mreq.imr_interface.s_addr == INADDR_ANY) {
			*ifpp = NULL;
    		} else {
			struct net_device *dev = NULL;

			dev = ip_dev_find(mreq.imr_interface.s_addr);
			if (!dev) {
				*ifpp = NULL;
			} else {
				__dev_put(dev);
				*ifpp = __in_dev_get(dev);
			}
			if (*ifpp == NULL)		
				return -EADDRNOTAVAIL;
		}

		gaddr->s_addr = mreq.imr_multiaddr.s_addr;	
		break;
	    }

	    case IP_BLOCK_SOURCE:
	    case IP_UNBLOCK_SOURCE:
	    case IP_ADD_SOURCE_MEMBERSHIP:
	    case IP_DROP_SOURCE_MEMBERSHIP: 
        {
		/*
		 * Argument is 'struct ip_mreq_source'
		 */
		struct ip_mreq_source mreqsrc;

		/*
 		 * Copy mreq_source struct from userspace
		 */
		if (optlen < sizeof(struct ip_mreq_source))
			return -EINVAL;		
		if (copy_from_user(&mreqsrc, optval, 
                    sizeof(struct ip_mreq_source))) {
			return -EFAULT;
                }

		/*
 		 * If interface specified, find it and hold a reference on it.
		 */
        if (mreqsrc.imr_interface.s_addr == INADDR_ANY) {
			*ifpp = NULL;
    		} else {
			struct net_device *dev = NULL;

			dev = ip_dev_find(mreqsrc.imr_interface.s_addr);
			if (!dev) {
				*ifpp = NULL;
			} else {
				__dev_put(dev);
				*ifpp = __in_dev_get(dev);
			}
			if (*ifpp == NULL)		
				return -EADDRNOTAVAIL;
		}

		gaddr->s_addr = mreqsrc.imr_multiaddr.s_addr;
		saddr->s_addr = mreqsrc.imr_sourceaddr.s_addr;
		break;
            }
	    
	    case MCAST_JOIN_GROUP:
	    case MCAST_LEAVE_GROUP: 
        case MCAST_BLOCK_SOURCE:
	    case MCAST_UNBLOCK_SOURCE:
	    case MCAST_JOIN_SOURCE_GROUP:
	    case MCAST_LEAVE_SOURCE_GROUP: 
		/*
		 * We don't support the protocol independent interface, yet.
		 */
		err = -EOPNOTSUPP;
		break;

	    default:
		err = -EOPNOTSUPP;
		break;
	}

    return err;
}

/*
 * Add a single address (ordered) to a source filter
 */
static int ip_sock_add_msf(struct sock_mcastsf *msf, struct in_addr src) 
{
	struct source_list *psrc, *next, *prev = NULL;

	igmp_printk(KERN_DEBUG "ip_sock_add_msf\n");

	for (next = msf->smsf_slist; next != NULL; next = next->src_next) {
		if (next->src_addr.s_addr == src.s_addr)
			return -EADDRNOTAVAIL;
		if (next->src_addr.s_addr > src.s_addr)
			break; 
		prev = next;
	}
	psrc = (struct source_list *) kmalloc(sizeof(*psrc), GFP_ATOMIC);
	if (psrc == NULL)
		return -ENOBUFS;
	psrc->src_addr = src;
	psrc->src_next = next;
	if (prev != NULL)
		prev->src_next = psrc;
	else
		msf->smsf_slist = psrc;
	msf->smsf_numsrc++;
	return 0;
}

/*
 * Remove a single address from a source filter
 */
static int ip_sock_remove_msf(struct sock_mcastsf *msf, struct in_addr src) 
{
	struct source_list *psrc, *psrc2 = NULL;
	int err = -EADDRNOTAVAIL;

	igmp_printk(KERN_DEBUG "ip_sock_remove_msf\n");

	for (psrc = msf->smsf_slist; psrc != NULL; psrc = psrc->src_next) {
		if (psrc->src_addr.s_addr == src.s_addr) {
			if (psrc2 == NULL)
				msf->smsf_slist = psrc->src_next;
			else
				psrc2->src_next = psrc->src_next;	
			kfree(psrc);
			msf->smsf_numsrc--;
			err = 0;
			break;
		}
		psrc2 = psrc;
	}
	return err;
}

/*
 * Set the membership-related IP multicast options in response to user
 * setsockopt().
 */
int ip_setmoptions(struct sock *sk, int optname, char *optval, int optlen)
{
  	int err = 0;	
	struct in_addr gaddr, saddr;
	struct in_device *ifp;  
   	int i;
	struct source_list sf;
 
	igmp_printk(KERN_DEBUG "ip_setmoptions\n");
 
   	switch (optname) 
    {
        case IP_ADD_MEMBERSHIP:
	    case MCAST_JOIN_GROUP:
		/* 
 		 * Add a multicast group membership, with source filter set to
		 * exclude{}
		 */
		if ((err = ip_get_sopt_mreq_arg(optname, optval, optlen, &gaddr,
                    &saddr, &ifp)) != 0)
			break;
		if (!MULTICAST(gaddr.s_addr)) 
        {
			err = -EINVAL;
			break;
		}
		igmp_printk(KERN_DEBUG "ip_setmoptions: taking igmp_lock\n");
		igmp_lock();
        if ((err = ip_addmembership(sk, &gaddr, ifp, &i)) == 0) 
        {
            sk->protinfo.af_inet.mc_membershipsf[i].smsf_fmode = MCAST_EXCLUDE;
            sk->protinfo.af_inet.mc_membershipsf[i].smsf_slist = NULL;
			/* Tell igmp: socket( inc{0} ) -> socket( exc{0} ) */
			igmp_chg_sourcefilter( sk->protinfo.af_inet.mc_membership[i], NULL, 
                            MCAST_INCLUDE, NULL, MCAST_EXCLUDE);
		}
		igmp_printk(KERN_DEBUG "ip_setmoptions: releasing igmp_lock\n");
		igmp_unlock();	
		break; 

	    case IP_ADD_SOURCE_MEMBERSHIP:
	    case MCAST_JOIN_SOURCE_GROUP:
		/*
 		 * Add a single source to a source filter of a socket in include
		 * mode, and, if the membership cannot be found, add membership.
		 */
		if ((err = ip_get_sopt_mreq_arg(optname, optval, optlen, &gaddr,
		    &saddr, &ifp)) != 0)
			break;
		if (!MULTICAST(gaddr.s_addr)) {
			err = -EINVAL;
			break;
		}
		igmp_printk(KERN_DEBUG "ip_setmoptions: taking igmp_lock\n");
		igmp_lock();
		err = ip_addmembership(sk, &gaddr, ifp, &i);
		/* 
         * Leave if current membership is in exclude mode
		 */
		if (err == -EADDRINUSE && 
		    sk->protinfo.af_inet.mc_membershipsf[i].smsf_fmode == MCAST_EXCLUDE) {
		     igmp_printk(KERN_DEBUG "ip_setmoptions: releasing igmp_lock\n");
		     igmp_unlock();	
		     err = -EINVAL;
		     break;
		}
		/*
		 * Add the source to the source filter
		 */
		if (err == 0 || err == -EADDRINUSE) {
			sf.src_addr = saddr;
			sf.src_next = NULL;
			sk->protinfo.af_inet.mc_membershipsf[i].smsf_fmode = MCAST_INCLUDE;
			err = ip_sock_add_msf(
                            &sk->protinfo.af_inet.mc_membershipsf[i], saddr);
			/*
			 * Tell igmp: socket( inc{S} ) -> socket( inc{S.s} ) 
			 */
			if (err == 0) 
				igmp_chg_sourcefilter(
                                    sk->protinfo.af_inet.mc_membership[i], NULL,
                                    MCAST_INCLUDE, &sf, MCAST_INCLUDE);
		}
		igmp_printk(KERN_DEBUG "ip_setmoptions: releasing igmp_lock\n");
		igmp_unlock();	
		break;

	    case IP_BLOCK_SOURCE:
	    case MCAST_BLOCK_SOURCE:
	    case IP_UNBLOCK_SOURCE:
	    case MCAST_UNBLOCK_SOURCE:
		/* 
		 * Add/remove a single source filter to/from a socket in 
		 * exclude mode.
		 */
		if ((err = ip_get_sopt_mreq_arg(optname, optval, optlen, &gaddr,
		    &saddr, &ifp)) != 0)
			break;

		if (!MULTICAST(gaddr.s_addr)) {
			err = -EINVAL;		
			break;
		}

		igmp_printk(KERN_DEBUG "ip_setmoptions: taking igmp_lock\n");
		igmp_lock();

		if ((err = ip_findmembership(sk, &gaddr, ifp, &i)) != 0) {
			err = -EADDRNOTAVAIL;
		     igmp_printk(KERN_DEBUG "ip_setmoptions: releasing igmp_lock\n");
		     igmp_unlock();	
	 	     break;
		}

		/*
		 * If the socket is in exclude mode, try to add/remove the
		 * source and, on success, tell igmp about the change.
		 */
		if (sk->protinfo.af_inet.mc_membershipsf[i].smsf_fmode == MCAST_EXCLUDE) {
			sf.src_addr = saddr;
			sf.src_next = NULL;
			if (optname == IP_BLOCK_SOURCE || optname == 
			    MCAST_BLOCK_SOURCE) {
				err = ip_sock_add_msf(
				    &sk->protinfo.af_inet.mc_membershipsf[i],
				    saddr);
				/*
                                 * Tell igmp: socket( exc{S} ) -> 
			         * socket( exc{S, s} )
				 */
				if (err == 0) {
				     igmp_chg_sourcefilter(
				       sk->protinfo.af_inet.mc_membership[i], 
				       NULL, MCAST_EXCLUDE, &sf, MCAST_EXCLUDE);
				}
			} else {
				err = ip_sock_remove_msf(
				    &sk->protinfo.af_inet.mc_membershipsf[i],
				    saddr);
				/*
				 * Tell igmp: socket( exc{S,s} ) ->
				 * socket( exc{S} )
				 */
				if (err == 0) {
				    igmp_chg_sourcefilter(
				      sk->protinfo.af_inet.mc_membership[i],
				      &sf, MCAST_EXCLUDE, NULL, MCAST_EXCLUDE);
				}
			}
                } else {
			err = -EINVAL;
		}
		igmp_printk(KERN_DEBUG "ip_setmoptions: releasing igmp_lock\n");
		igmp_unlock();	
 		break;

	    case IP_DROP_SOURCE_MEMBERSHIP:
	    case MCAST_LEAVE_SOURCE_GROUP:
		/*
		 * Drop a source from a source filter of a socket in include
		 * mode, and drop membership when there are no sources left in
		 * that filter
		 */
		if ((err = ip_get_sopt_mreq_arg(optname, optval, optlen, &gaddr,
		    &saddr, &ifp)) != 0)
			break;

		if (!MULTICAST(gaddr.s_addr)) {
			err = -EINVAL;		
			break;
		}

		igmp_printk(KERN_DEBUG "ip_setmoptions: taking igmp_lock\n");
		igmp_lock();

		if ((err = ip_findmembership(sk, &gaddr, ifp, &i)) != 0) {
			err = -EADDRNOTAVAIL;
		     igmp_printk(KERN_DEBUG "ip_setmoptions: releasing igmp_lock\n");
		     igmp_unlock(); 
		     break;
		}

		if (sk->protinfo.af_inet.mc_membershipsf[i].smsf_fmode == MCAST_INCLUDE) {
			sf.src_addr = saddr;
			sf.src_next = NULL;
			err = ip_sock_remove_msf(
 			  	  &sk->protinfo.af_inet.mc_membershipsf[i],
				  saddr);
			if (err == 0) {
				struct sock_mcastsf msf;

				/* 
				 * Tell igmp: socket( inc{S,s} ) ->
			 	 * socket( inc{S} )
				 */
				igmp_chg_sourcefilter(
				  sk->protinfo.af_inet.mc_membership[i], &sf,
				  MCAST_INCLUDE, NULL, MCAST_INCLUDE);
	
				/*
				 * If the new filter state is inc{0}, give up
				 * membership.
				 */
				msf = sk->protinfo.af_inet.mc_membershipsf[i];
				if (msf.smsf_numsrc == 0)	
					err = ip_dropmembership(sk, &gaddr, ifp);
			}
		} else {
			err = -EINVAL;
		}
		igmp_printk(KERN_DEBUG "ip_setmoptions: releasing igmp_lock\n");
		igmp_unlock();	
		break;

	    case IP_DROP_MEMBERSHIP:
	    case MCAST_LEAVE_GROUP:
		/*
		 * Drop a multicast group membership.
		 * Group must be a valid IP multicast address.
		 */
		if ((err = ip_get_sopt_mreq_arg(optname, optval, optlen, &gaddr,
		    &saddr, &ifp)) != 0)
			break;

		if (!MULTICAST(gaddr.s_addr)) {
			err = -EINVAL;		
			break;
		}

		igmp_printk(KERN_DEBUG "ip_setmoptions: taking igmp_lock\n");
		igmp_lock();

		if ((err = ip_findmembership(sk, &gaddr, ifp, &i)) != 0) {
		    err = -EADDRNOTAVAIL;
		    igmp_printk(KERN_DEBUG "ip_setmoptions: releasing igmp_lock\n");
		    igmp_unlock();	
		    break;
		}

		/*
		 * Tell igmp: socket( exc{S}|inc{S,s} ) -> socket( inc{0} )
		 */
		igmp_chg_sourcefilter(sk->protinfo.af_inet.mc_membership[i],
			sk->protinfo.af_inet.mc_membershipsf[i].smsf_slist,
			sk->protinfo.af_inet.mc_membershipsf[i].smsf_fmode,
			NULL, MCAST_INCLUDE);
		err = ip_dropmembership(sk, &gaddr, ifp);
		igmp_printk(KERN_DEBUG "ip_setmoptions: releasing igmp_lock\n");
		igmp_unlock();	
		break;

	    default:
		err = -EOPNOTSUPP;
		break;
	}

	return err;
}

/*
 * Set multicast source filter of a socket (SIOCSIPMSFILTER)
 */
int in_setmcast_srcfilter(struct sock *sk, unsigned long arg)
{
	struct ip_msfilter imsf, *imsfp;
	int err, i, j;
	struct in_device *ifp;
	struct source_list *slist, *psrc, *psrc2;
	struct in_addr ip;

	igmp_printk(KERN_DEBUG "in_setmcast_srcfilter\n");

	/*
 	 * Get and verify arguments
	 */
	imsfp = (struct ip_msfilter *) arg;
	err = copy_from_user(&imsf, imsfp, IP_MSFILTER_SIZE(0));
	if (err != 0 || imsf.imsf_numsrc > IP_MAX_SRCFILTERS || 
	    !MULTICAST(imsf.imsf_multiaddr.s_addr))
		return -EINVAL;

	/*
	 * Determine the interface.  We're left holding a reference to it.
	 */
	if (imsf.imsf_interface.s_addr == INADDR_ANY)
		ifp = NULL;
	else {
        	struct net_device *dev = NULL;

            dev = ip_dev_find(imsf.imsf_interface.s_addr);
            if (!dev) {
                ifp = NULL;
            } else {
                    __dev_put(dev);
                    ifp = __in_dev_get(dev);
            }
		if (ifp == NULL)
			return -EADDRNOTAVAIL;	
	}

	igmp_lock();

	/*
	 * Find the group entry
	 */
	for (i = 0; i < sk->protinfo.af_inet.mc_num_memberships; i++) {
		if (sk->protinfo.af_inet.mc_membership[i]->inm_addr.s_addr ==
		    imsf.imsf_multiaddr.s_addr && (ifp == NULL ||
		    sk->protinfo.af_inet.mc_membership[i]->inm_ifp == ifp))
			break;
	}

	/*
	 * Not found, try to add membership
	 */
	if (i == sk->protinfo.af_inet.mc_num_memberships) {
		err = ip_addmembership(sk, &imsf.imsf_multiaddr, ifp, &i);
		if (err) {
			igmp_unlock();
			return err;
		}
	}

	/*
	 * Create the list
	 */
	slist = sk->protinfo.af_inet.mc_membershipsf[i].smsf_slist;
	sk->protinfo.af_inet.mc_membershipsf[i].smsf_slist = NULL;
	for (j = 0; j < imsf.imsf_numsrc; j++) {
		/*
		 * Get the ip address from user space 
		 */
		err = copy_from_user(&ip, &imsfp->imsf_slist[j], sizeof(ip));
		if (err == 0) 
			err = ip_sock_add_msf(
				&sk->protinfo.af_inet.mc_membershipsf[i], ip);
		/*
		 * If an error occured, undo all changes.
		 */
		if (err != 0) {
		    psrc = sk->protinfo.af_inet.mc_membershipsf[i].smsf_slist;
	 	    while (psrc != NULL) {
			psrc2 = psrc->src_next;
			kfree(psrc);
			psrc = psrc2;
		    }
		    sk->protinfo.af_inet.mc_membershipsf[i].smsf_slist=slist;
		    igmp_unlock();	
		    return err;
		} 
	}
	
	/*
	 * Tell igmp about the sourcefilter change of this socket
	 */	
	igmp_chg_sourcefilter(sk->protinfo.af_inet.mc_membership[i], slist,
		sk->protinfo.af_inet.mc_membershipsf[i].smsf_fmode,
		sk->protinfo.af_inet.mc_membershipsf[i].smsf_slist,
		imsf.imsf_fmode);

	/*
	 * Drop membership if the new filter is include{0}, else assign new
	 * filter.
	 */
	if (imsf.imsf_numsrc == 0 && imsf.imsf_fmode == MCAST_INCLUDE) {
		ip_dropmembership(sk, &imsf.imsf_multiaddr, ifp);
	} else {
		sk->protinfo.af_inet.mc_membershipsf[i].smsf_fmode = 
			imsf.imsf_fmode;
		sk->protinfo.af_inet.mc_membershipsf[i].smsf_numsrc =
			imsf.imsf_numsrc;
	}

	/*
	 * Delete the old filterlist
	 */
	for (psrc = slist; psrc != NULL; psrc = psrc2) {
		psrc2 = psrc->src_next;
		kfree(psrc);
	}

	igmp_unlock();	
	return 0;
}

/*
 * Get multicast source filter of a socket (SIOCGIPMSFILTER)
 */
int in_getmcast_srcfilter(struct sock *sk, unsigned long arg)
{
	struct ip_msfilter imsf, *imsfp;
	int err, i, j, max;
	struct in_device *ifp;
	struct source_list *psrc;
	struct in_addr ip;

	igmp_printk(KERN_DEBUG "in_getmcast_srcfilter\n");

	/*
 	 * Get and verify arguments
	 */
	imsfp = (struct ip_msfilter *) arg;
	err = copy_from_user(&imsf, imsfp, IP_MSFILTER_SIZE(0));
	if (err != 0 || imsf.imsf_numsrc < 0 || 
	    !MULTICAST(imsf.imsf_multiaddr.s_addr))
		return -EINVAL;

	/*
	 * Determine the interface.  We're left holding a reference to it.
	 */
	if (imsf.imsf_interface.s_addr == INADDR_ANY)
		ifp = NULL;
	else {
        	struct net_device *dev = NULL;

                dev = ip_dev_find(imsf.imsf_interface.s_addr);
                if (!dev) {
	                ifp = NULL;
                } else {
                        __dev_put(dev);
                        ifp = __in_dev_get(dev);
                }
		if (ifp == NULL)
			return -EADDRNOTAVAIL;	
	}

	igmp_lock();

	/*
	 * Find the group entry
	 */
	for (i = 0; i < sk->protinfo.af_inet.mc_num_memberships; i++) {
		if (sk->protinfo.af_inet.mc_membership[i]->inm_addr.s_addr ==
		    imsf.imsf_multiaddr.s_addr && (ifp == NULL ||
		    sk->protinfo.af_inet.mc_membership[i]->inm_ifp == ifp))
			break;
	}
	if (i == sk->protinfo.af_inet.mc_num_memberships)
		return -EADDRNOTAVAIL;

	/*
	 * Fill the buffer and set the rest of the structure
	 */
	max = imsf.imsf_numsrc;
	j = 0;
	psrc = sk->protinfo.af_inet.mc_membershipsf[i].smsf_slist;
	while (psrc != NULL && j < max) {
	 	ip = psrc->src_addr;	
		err = copy_to_user(&imsfp->imsf_slist[j++], &ip, sizeof(ip)); 
		if (err != 0) {
		        igmp_unlock();
			return err;
		}
		psrc = psrc->src_next;
	}
	imsf.imsf_numsrc = sk->protinfo.af_inet.mc_membershipsf[i].smsf_numsrc;
	if (sk->protinfo.af_inet.mc_membershipsf[i].smsf_slist != NULL)
		imsf.imsf_fmode = 
			sk->protinfo.af_inet.mc_membershipsf[i].smsf_fmode;
	else
		imsf.imsf_fmode = MCAST_EXCLUDE;
	err = copy_to_user(imsfp, &imsf, IP_MSFILTER_SIZE(0));
	igmp_unlock();
	return err;
}

#if defined(CONFIG_RG_IGMP_PROXY) || defined (CONFIG_RG_IGMP_PROXY_MODULE)

void igmp_print_inmp(struct in_multi *inmp)
{
    igmp_lock();
    if (inmp != NULL)
        __igmp_print_inmp(inmp);
    igmp_unlock();
}

static void igmp_startup_statechg_report_set(struct in_multi *inm, unsigned int mcaddr, int value)
{
	if (igmp_reserved_address(mcaddr))
		return;

	inm->inm_startup_statechg_report = value;
}

struct in_multi *igmp_add_membership(struct in_device *in_dev, unsigned int group)
{
    struct in_addr gaddr;
    struct in_multi *inm;

	igmp_printk(KERN_DEBUG "igmp_add_membership\n");
    gaddr.s_addr = group;

    igmp_lock();
    inm = in_createmulti(&gaddr, in_dev);
    if (!inm)
    {
        igmp_unlock();
        return NULL;
    }

#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
    if (inm && inm->users == 1)
    {
#if defined(CONFIG_RG_IGMP_PROXY) || defined(CONFIG_RG_IGMP_PROXY_MODULE)

#ifdef CONFIG_RG_IGMP_PROXY
        if (igmprx_upstream_filter(in_dev->dev->ifindex, group))
			igmp_startup_statechg_report_set(inm, group, 1);
#endif /* CONFIG_RG_IGMP_PROXY */

#ifdef CONFIG_RG_IGMP_PROXY_MODULE
        if (igmp_proxy_upstream_filter)
        {
            if (igmp_proxy_upstream_filter(in_dev->dev->ifindex, group))
				igmp_startup_statechg_report_set(inm, group, 1);
        }
        else
        {
			igmp_startup_statechg_report_set(inm, group, 1);
        }
#endif /* CONFIG_RG_IGMP_PROXY_MODULE */

#else
		igmp_startup_statechg_report_set(inm, group, 1);
#endif /* (CONFIG_RG_IGMP_PROXY) || (CONFIG_RG_IGMP_PROXY_MODULE) */
    }
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */
    igmp_unlock();

    return (inm);
}

void igmp_remove_membership(struct in_device *in_dev, unsigned int group)
{
    struct in_multi *inm;

	igmp_printk(KERN_DEBUG "igmp_remove_membership\n");
	igmp_lock();

    read_lock(&in_dev->lock); 
	for (inm = in_dev->mc_list; inm; inm = inm->inm_next) 
    {
		if (inm->inm_addr.s_addr == group) 
            break;
    }
    read_unlock(&in_dev->lock);

    if (!inm)
    {
        igmp_unlock();
        return;
    }

    in_destroymulti(inm);

    igmp_unlock();
}

int igmp_update_membership(struct in_device *in_dev, unsigned int group,
                           struct source_list *src_rem, int fmode_from, 
                           struct source_list *src_add, int fmode_to)
{
    int status;
    struct in_multi *inm;

	igmp_printk(KERN_DEBUG "igmp_update_membership\n");
	igmp_lock();

    read_lock(&in_dev->lock); 
	for (inm = in_dev->mc_list; inm; inm = inm->inm_next) 
    {
		if (inm->inm_addr.s_addr == group) 
            break;
    }
    read_unlock(&in_dev->lock);

    if (!inm)
    {
        igmp_unlock();
        return 0;
    }

    __igmp_print_inmp(inm);
    status = igmp_chg_sourcefilter(inm, src_rem, fmode_from, src_add, fmode_to);
    __igmp_print_inmp(inm);

    igmp_unlock();

    return status;
}

void igmp_update_last_report_time(struct in_device *in_dev, unsigned int group)
{
    struct timeval tv;
    struct in_multi *inm;

	igmp_lock();

    read_lock(&in_dev->lock); 
	for (inm = in_dev->mc_list; inm; inm = inm->inm_next) 
    {
		if (inm->inm_addr.s_addr == group) 
            break;
    }
    read_unlock(&in_dev->lock);

    if (inm)
    {
        do_gettimeofday(&tv);
        inm->inm_last_report_time = tv.tv_sec;
    }

    igmp_unlock();
}

void igmp_update_total_joins(struct in_device *in_dev, unsigned int group)
{
    struct in_multi *inm;

	igmp_lock();

    read_lock(&in_dev->lock); 
	for (inm = in_dev->mc_list; inm; inm = inm->inm_next) 
    {
		if (inm->inm_addr.s_addr == group) 
		{
        	++inm->inm_total_joins;
            break;
		}
    }
    read_unlock(&in_dev->lock);

    igmp_unlock();
}


void igmp_update_total_leaves(struct in_device *in_dev, unsigned int group)
{
    struct in_multi *inm;

	igmp_lock();

    read_lock(&in_dev->lock); 
	for (inm = in_dev->mc_list; inm; inm = inm->inm_next) 
    {
		if (inm->inm_addr.s_addr == group) 
		{
        	++inm->inm_total_leaves;
            break;
		}
    }
    read_unlock(&in_dev->lock);

    igmp_unlock();
}

#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
void igmp_update_startup_statechg_report(struct in_device *in_dev, unsigned int group)
{
    struct in_multi *inm;

	igmp_lock();

    read_lock(&in_dev->lock); 
	for (inm = in_dev->mc_list; inm; inm = inm->inm_next) 
    {
		if (inm->inm_addr.s_addr == group) 
		{
			if (inm->inm_startup_statechg_report == 1)
			{
				inm->inm_startup_statechg_report = 0;
                inm->inm_ti_statechg_interval = igmp_host_unsol_interval;
			}
            break;
		}
    }
    read_unlock(&in_dev->lock);

    igmp_unlock();
}
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */

void igmp_inc_group_self(struct in_device *in_dev, unsigned int group)
{
    struct in_multi *inm;

	igmp_lock();

    read_lock(&in_dev->lock); 
	for (inm = in_dev->mc_list; inm; inm = inm->inm_next) 
    {
		if (inm->inm_addr.s_addr == group) 
		{
        	++inm->inm_grp_self;
            break;
		}
    }
    read_unlock(&in_dev->lock);

    igmp_unlock();
}

void igmp_dec_group_self(struct in_device *in_dev, unsigned int group)
{
    struct in_multi *inm;

	igmp_lock();

    read_lock(&in_dev->lock); 
	for (inm = in_dev->mc_list; inm; inm = inm->inm_next) 
    {
		if (inm->inm_addr.s_addr == group) 
		{
        	--inm->inm_grp_self;
            break;
		}
    }
    read_unlock(&in_dev->lock);

    igmp_unlock();
}

unsigned int igmp_get_inm_users(struct in_device *in_dev, unsigned int group)
{
    struct in_multi *inm = NULL;

	igmp_lock();
    read_lock(&in_dev->lock); 
	for (inm = in_dev->mc_list; inm; inm = inm->inm_next) 
    {
		if (inm->inm_addr.s_addr == group) 
		{
    		read_unlock(&in_dev->lock);
    		igmp_unlock();
        	return(inm->users);
		}
    }
    read_unlock(&in_dev->lock);
    igmp_unlock();

	return 0;
}

void igmp_set_client_unsol_interval(int unsol_interval)
{
	igmp_lock();
    igmp_host_unsol_interval = unsol_interval;
	igmp_printk(KERN_DEBUG "igmp_set_client_unsol_interval: New interval=%d\n", igmp_host_unsol_interval);
    igmp_unlock();
}

int igmp_get_client_unsol_interval(void)
{
    int unsol_interval;
	igmp_lock();
    unsol_interval = igmp_host_unsol_interval;
    igmp_unlock();
    return (unsol_interval);
}

#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
void igmp_set_persistent_join_interval(int persis_join_interval)
{
	igmp_lock();
    igmp_persistent_join_interval = persis_join_interval;
	igmp_printk(KERN_DEBUG "igmp_set_persistent_join_interval: New interval=%d\n", igmp_persistent_join_interval);
    igmp_unlock();
}

int igmp_get_persistent_join_interval(void)
{
    int persis_join_interval;
	igmp_lock();
    persis_join_interval = igmp_persistent_join_interval;
    igmp_unlock();
    return (persis_join_interval);
}
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */

int igmp_get_client_version(void)
{
    return (igmp_host_version);
}

/* Functions to support GUI/TR-069 for Group Table */

int igmp_host_max_group_table_size(void)
{
    int max_tbl_size = DEFAULT_MAX_MEMBERSHIPS;
    return (max_tbl_size);
}

/* Get Group Table size */
int igmp_host_group_table_size(struct in_device *in_dev)
{
    int grp_table_size;

	igmp_lock();
    grp_table_size = in_dev->mcgrp_count;
    igmp_unlock();

    return (grp_table_size);
}

/* 
 * Get next index in Group Table 
 * Return Value:
 *      >0 - Valid next index was found
 *      <=0 - No next index found
 */
int igmp_host_get_group_table_next_index(struct in_device *in_dev, int index)
{
    int count, next_index;
    struct in_multi *inm;

	igmp_lock();

    count = 0;
    next_index = 0;

    read_lock(&in_dev->lock); 
    inm = in_dev->mc_list;
    while (inm != NULL)
    {
        if (count > index)
        {
            next_index = count;
            break;
        }

        ++count;
        inm = inm->inm_next;
    }
    read_unlock(&in_dev->lock);

    igmp_unlock();

    return (next_index);
}

/* 
 * Get Group entry corresponding to an index in the Group Table 
 * Index is passed in 'grpinfo'
 * Group entry objects are also returned in 'grpinfo'
 */
int igmp_host_get_group_table_entry(struct in_device *in_dev, 
                                    struct igmp_host_group_req_t *grp_entry)
{
    int count;
    struct in_multi *inm=NULL;

	igmp_lock();

    count = 0;
    read_lock(&in_dev->lock); 
	inm = in_dev->mc_list;
    while (inm != NULL)
    {
        if (count >= grp_entry->tbl_index)
            break;
        ++count;
        inm = inm->inm_next;
    }
    read_unlock(&in_dev->lock);

    if (!inm)
    {
        igmp_unlock();
        return -1;
    }

    grp_entry->grp_addr = inm->inm_addr.s_addr;
    if (inm->inm_grp_self)
        grp_entry->grp_self = IGMP_HOST_GROUP_SELF_YES;
    else
        grp_entry->grp_self = IGMP_HOST_GROUP_SELF_NO;
    grp_entry->grp_rtr_intf_num_entries = inm->users;

    igmp_unlock();

    return (0);
}

/*
 * Retrieves entries for displaying Group entries in GUI
 */
int igmp_host_get_gui_group_table_entries(struct in_device *in_dev, 
                                          struct igmp_host_gui_group_req_t *req)
{
    int i, j, num_entries=0;
    struct in_multi *inm=NULL;

	igmp_lock();

    i = 0;
    read_lock(&in_dev->lock); 
	inm = in_dev->mc_list;
    while (inm != NULL)
    {
        struct in_sfentry *sf;
        struct igmp_host_gui_group_entry_t *grp;

        if (i < req->grp_start_index)
        {
            ++i;
            inm = inm->inm_next;
            continue;
        }

        if (num_entries >= req->grp_requested_num_entries)
            break;

        /* Copy Group information */
        grp = &req->grp_entry[num_entries];

        strncpy(grp->ifname, in_dev->dev->name, IFNAMSIZ);
        grp->mcaddr = inm->inm_addr.s_addr;
        grp->filter_mode = inm->inm_fmode;

        j=0;
        SF_START(inm->inm_fmode, inm, sf);
        if (sf)
        {
            for ( ; sf; sf = sf->isf_next) 
            {
                if (sf->isf_refcount != 0) 
                {
                    grp->srclist[j++] = sf->isf_addr.s_addr;
                    if (j >= IGMP_HOST_REQ_MAX_SOURCES)
                        break;
                }
            }	
        }
        grp->src_count = j;
	
        ++num_entries;
        ++i;
        inm = inm->inm_next;
    }
    read_unlock(&in_dev->lock);

    req->grp_actual_num_entries = num_entries;

    igmp_unlock();

    return (0);
}

/* Functions to support GUI/TR-069 for Group Stats Table */

/* Get Group Stats Table size */
int igmp_host_groupstats_table_size(struct in_device *in_dev)
{
    int grp_table_size;

	igmp_lock();
    grp_table_size = in_dev->mcgrp_count;
    igmp_unlock();

    return (grp_table_size);
}

/* 
 * Get next index in Group Stats Table 
 * Return Value:
 *      >0 - Valid next index was found
 *      <=0 - No next index found
 */
int igmp_host_get_groupstats_table_next_index(struct in_device *in_dev, int index)
{
    int count, next_index;
    struct in_multi *inm;

	igmp_lock();

    count = 0;
    next_index = 0;

    read_lock(&in_dev->lock); 
    inm = in_dev->mc_list;
    while (inm != NULL)
    {
        if (count > index)
        {
            next_index = count;
            break;
        }

        ++count;
        inm = inm->inm_next;
    }
    read_unlock(&in_dev->lock);

    igmp_unlock();

    return (next_index);
}

/* 
 * Get Group entry corresponding to an index in the Group Table 
 * Index is passed in 'grpinfo'
 * Group entry objects are also returned in 'grpinfo'
 */
int igmp_host_get_groupstats_table_entry(struct in_device *in_dev, 
                                         struct igmp_host_groupstats_req_t *grp_entry)
{
    int count;
    struct in_multi *inm=NULL;

	igmp_lock();

    count = 0;
    read_lock(&in_dev->lock); 
	inm = in_dev->mc_list;
    while (inm != NULL)
    {
        if (count >= grp_entry->tbl_index)
            break;
        ++count;
        inm = inm->inm_next;
    }
    read_unlock(&in_dev->lock);

    if (!inm)
    {
        igmp_unlock();
        return -1;
    }

    grp_entry->grp_addr = inm->inm_addr.s_addr;
    grp_entry->last_report_time = inm->inm_last_report_time;
    grp_entry->total_start = (jiffies - inm->inm_start_total_stats)/HZ;
    grp_entry->total_joins = inm->inm_total_joins;
    grp_entry->total_leaves = inm->inm_total_leaves;

    igmp_unlock();

    return (0);
}

/*
 * Retrieves entries for displaying Group Stat entries in GUI
 */
int igmp_host_get_gui_groupstat_table_entries(struct in_device *in_dev, 
                                          struct igmp_host_gui_groupstat_req_t *req)
{
    int i, num_entries=0;
    struct in_multi *inm=NULL;

	igmp_lock();

    i = 0;
    read_lock(&in_dev->lock); 
	inm = in_dev->mc_list;
    while (inm != NULL)
    {
        struct igmp_host_gui_groupstat_entry_t *grp;

        if (i < req->grp_start_index)
        {
            ++i;
            inm = inm->inm_next;
            continue;
        }

        if (num_entries >= req->grp_requested_num_entries)
            break;

        /* Copy Group information */
        grp = &req->grpstat_entry[num_entries];

        strncpy(grp->ifname, in_dev->dev->name, IFNAMSIZ);
        grp->mcaddr = inm->inm_addr.s_addr;

        grp->grp_last_report_time = inm->inm_last_report_time;
        grp->grp_start_total_stats = (jiffies - inm->inm_start_total_stats)/HZ;
        grp->grp_total_joins = inm->inm_total_joins;
        grp->grp_total_leaves = inm->inm_total_leaves;

        ++num_entries;
        ++i;
        inm = inm->inm_next;
    }
    read_unlock(&in_dev->lock);

    req->grp_actual_num_entries = num_entries;

    igmp_unlock();

    return (0);
}

#endif /* defined(CONFIG_RG_IGMP_PROXY) || defined (CONFIG_RG_IGMP_PROXY_MODULE) */

#endif /* ACTION_TEC_IGMPV3 */

