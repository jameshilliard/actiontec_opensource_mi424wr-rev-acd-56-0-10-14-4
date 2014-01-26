/*
 *	Linux NET3:	Internet Group Management Protocol  [IGMP]
 *
 *	Stub file.  Selects the proper version of IGMP for inclusion based
 * 	on the CONFIG_IP_IGMPV3 configuration setting.
 *
 *      Author:
 *		Vince Laviano <vlaviano@cisco.com>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#ifndef _LINUX_IGMP_H
#define _LINUX_IGMP_H

#include <linux/config.h>

/*
 * Include defs for appropriate version of IGMP
 */
#ifdef ACTION_TEC_IGMPV3

/*
 *      Internet Group Management Protocol Version 3 (IGMPv3)
 *
 *      This code implements the IGMPv3 protocol as defined in 
 *      draft-ietf-idmr-igmp-v3-11.txt.  IGMPv3 is backwards compatible with 
 *      IGMPv2 as defined in RFC 2236 and with IGMPv1 as defined in RFC 1112.  
 *      To enable IGMPv3, set CONFIG_IP_MULTICAST and CONFIG_IP_IGMPV3.
 *
 *	Version: $Id: igmp.h,v 1.10 2009/07/12 23:33:26 athill Exp $
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

#ifdef __KERNEL__
#include <linux/in.h>
#endif /* __KERNEL__ */

/*
 *	IGMP protocol structures
 */

/*
 * IGMP packet format
 */
struct igmphdr
{
	__u8  type;     /* version and type of IGMP message */
	__u8  code;		/* subtype for routing messages */
	__u16 csum;		/* IP-style checksum */
	__u32 group;	/* group address being reported (zero for queries) */
};

/*
 * IGMPv3 query format
 */
struct igmpv3 
{
	__u8  igmp_type;	/* version and type of IGMP message */
    __u8  igmp_code;	/* subtype for routing messages */
	__u16 igmp_cksum;	/* IP-style checksum */
	__u32 igmp_group;	/* group address being reported */
	__u8  igmp_misc;	/* reserved, suppress and robustness variable */
    __u8  igmp_qqi;     /* querier's query interval */
    __u16 igmp_numsrc;	/* number of sources */
	__u32 igmp_sources[1];  /* source addresses */
};

/* Fetch the [Robustness Variable] */
#define IGMP_QRV(x)	        ((x)->igmp_misc & (0x07))	

/*
 * IGMPv3 group record
 */
struct igmp_grouprec 
{
 	__u8  ig_type;		/* record type */
	__u8  ig_datalen;	/* amount of aux data */
	__u16 ig_numsrc;	/* number of sources */
	__u32 ig_group;		/* the group being reported */
    __u32 ig_sources[1];	/* source addresses */
};

/*
 * IGMPv3 report
 */
struct igmp_report 
{
	__u8  ir_type;		/* record type */
	__u8  ir_rsv1;		/* reserved */
	__u16 ir_cksum;		/* checksum */
	__u16 ir_rsv2;		/* reserved */
	__u16 ir_numgrps;	/* number of group records */
  	struct igmp_grouprec ir_groups[1]; 	/* group records */
};

#define IGMP_MINLEN			8
#define IGMP_HDRLEN			8
#define IGMPV3_HDRLEN		12
#define IGMP_GRPREC_HDRLEN		8
#define IGMP_PREPEND			0 	/* XXX irrelevant for Linux
						   impl since mbufs not used? */

/*
 * Maximum number of sources that can be specified in an igmp query, 
 * considering its length.
 */
#define IGMP_MAXSOURCES(len)	(((len)-12)>>2)

/*
 * Message types, including version number.
 * XXX Maintain old names in parallel with new ones for backwards compat?
 */
#define IGMP_MEMBERSHIP_QUERY		    0x11    /* membership query */
#define IGMP_V1_MEMBERSHIP_REPORT       0x12    /* Ver. 1 membership report */
#define IGMP_V2_MEMBERSHIP_REPORT       0x16    /* Ver. 2 membership report */
#define IGMP_V3_MEMBERSHIP_REPORT       0x22	/* Ver. 3 membership report */

#define IGMP_V2_LEAVE_GROUP             0x17	/* Leave-group message */

#define IGMP_DVMRP			0x13	/* DVMRP routing message */
#define IGMP_PIM			0x14	/* PIM routing message */

#define IGMP_TRACE			0x15
#define IGMP_MTRACE_RESP	0x1e  /* traceroute resp. (to sender) */
#define IGMP_MTRACE			0x1f  /* mcast traceroute messages */

/* 
 * The following four definitions are for backwards compatibility.  They
 * should be removed as soon as all applications are updated to use the new
 * constant names.
 */
#define IGMP_HOST_MEMBERSHIP_QUERY      IGMP_MEMBERSHIP_QUERY
#define IGMP_HOST_MEMBERSHIP_REPORT     IGMP_V1_MEMBERSHIP_REPORT
#define IGMP_HOST_NEW_MEMBERSHIP_REPORT IGMP_V2_MEMBERSHIP_REPORT
#define IGMP_HOST_LEAVE_MESSAGE         IGMP_V2_LEAVE_GROUP

/*
 *	Use the BSD names for these for compatibility
 */
#define IGMP_DELAYING_MEMBER		0x01
#define IGMP_IDLE_MEMBER		    0x02
#define IGMP_LAZY_MEMBER		    0x03
#define IGMP_SLEEPING_MEMBER		0x04
#define IGMP_AWAKENING_MEMBER		0x05

#define IGMP_MAX_HOST_REPORT_DELAY	10	/* max delay for response to */
						                /* query (in seconds)	*/
#define IGMP_TIMER_SCALE		10	/* denotes that the igmphdr->timer field */
						            /* specifies time in 10th of seconds	 */


#define IGMP_ALL_HOSTS		htonl(0xE0000001L)
#define IGMP_ALL_ROUTER 	htonl(0xE0000002L)
#define IGMPV3_ALL_ROUTER 	htonl(0xE0000016L)
#define IGMP_LOCAL_GROUP	htonl(0xE0000000L)
#define IGMP_LOCAL_GROUP_MASK	htonl(0xFFFFFF00L)

struct igmpstat 
{
	__u32	igps_rcv_total;		    /* total IGMP messages received */
	__u32	igps_rcv_tooshort;	    /* received with too few bytes */
	__u32	igps_rcv_toolong;	    /* received with too many bytes */
	__u32	igps_rcv_badsum;	    /* received with bad checksum */

	__u32	igps_rcv_queries;	    /* received membership queries */
	__u32	igps_rcv_badqueries;	/* received invalid queries */

	__u32 	igps_rcv_v2_reports;	/* received IGMPv1/v2 Reports/Joins */
	__u32 	igps_rcv_v2_leaves;	    /* received IGMPv2 Leaves */
	__u32	igps_rcv_v2_badreports;	/* received invalid reports */
	__u32	igps_rcv_v2_ourreports;	/* received reports for our groups */
	__u32 	igps_rcv_v3_reports;    /* received IGMPv3 Reports */

	__u32	igps_snd_reports;
	__u32	igps_snd_v1_joins;
	__u32	igps_snd_v2_joins;
	__u32	igps_snd_v2_leaves;
	__u32	igps_snd_v3_reports;
	__u32	igps_snd_v3_mode_in_reports;
	__u32	igps_snd_v3_mode_ex_reports;
	__u32	igps_snd_v3_to_in_reports;
	__u32	igps_snd_v3_to_ex_reports;
	__u32	igps_snd_v3_allow_reports;
	__u32	igps_snd_v3_block_reports;
};

#define IGMP_HOST_GROUP_SELF_YES    1
#define IGMP_HOST_GROUP_SELF_NO     0
struct igmp_host_group_req_t 
{
    char ifname[16]; /* IFNAMSIZ */
    int tbl_index;
    unsigned int grp_table_size;

    /* Group Table Entry */
    unsigned int grp_addr;
    int grp_self;
    unsigned int grp_rtr_intf_num_entries;
};

struct igmp_host_groupstats_req_t 
{
    char ifname[16]; /* IFNAMSIZ */
    int tbl_index;
    unsigned int grp_table_size;

    /* Group Stats Table Entry */
    unsigned int grp_addr;
    unsigned int last_report_time;
    unsigned int total_start;
    unsigned int total_joins;
    unsigned int total_leaves;
};

#define IGMP_HOST_REQ_MAX_SOURCES       8
struct igmp_host_gui_group_entry_t 
{
    char ifname[16]; /* IFNAMSIZ */
    unsigned int mcaddr;
    int filter_mode;
    int src_count;
    unsigned int srclist[IGMP_HOST_REQ_MAX_SOURCES];
};

#define IGMP_HOST_REQ_MAX_GROUPS        8
struct igmp_host_gui_group_req_t 
{
    /* Upstream/WAN interface */
    char ifname[16]; /* IFNAMSIZ */
    /* Retrieve entry starting at this index */
    int grp_start_index;
    /* Number of entries requested */
    int grp_requested_num_entries;

    /* Actual number of entries returned */
    int grp_actual_num_entries;
    /* Group Table Entry */
    struct igmp_host_gui_group_entry_t grp_entry[IGMP_HOST_REQ_MAX_GROUPS];
};

struct igmp_host_gui_groupstat_entry_t 
{
    char ifname[16]; /* IFNAMSIZ */
    unsigned int mcaddr;
    unsigned int grp_last_report_time;
    unsigned int grp_start_total_stats;
    unsigned int grp_total_joins;
    unsigned int grp_total_leaves;
};

struct igmp_host_gui_groupstat_req_t 
{
    /* Upstream/WAN interface */
    char ifname[16]; /* IFNAMSIZ */
    /* Retrieve entry starting at this index */
    int grp_start_index;
    /* Number of entries requested */
    int grp_requested_num_entries;

    /* Actual number of entries returned */
    int grp_actual_num_entries;
    /* Group Stat Table Entry */
    struct igmp_host_gui_groupstat_entry_t grpstat_entry[IGMP_HOST_REQ_MAX_GROUPS];
};


/*
 * struct for keeping the multicast list in
 */

#ifdef __KERNEL__

#define IGMP_RANDOM_DELAY(X) ((net_random() % (X)) + 1)

/*
 * States masks for IGMPv3
 */
#define IGMP_NONEXISTENT		0x01
#define IGMP_OTHERMEMBER		0x02
#define IGMP_IREPORTEDLAST		0x04

/*
 * We must remember what version the subnet's querier is.
 * We conveniently use the IGMP message type for the proper membership report
 * to keep this state.
 */
#define IGMP_V1_ROUTER			IGMP_V1_MEMBERSHIP_REPORT
#define IGMP_V2_ROUTER			IGMP_V2_MEMBERSHIP_REPORT
#define IGMP_V3_ROUTER			IGMP_V3_MEMBERSHIP_REPORT

#define IGMP_V1_HOST			1
#define IGMP_V2_HOST			2
#define IGMP_V3_HOST			3

/*
 * Revert to new router if we haven't heard from an old router in this
 * amount of time.
 * XXX units?  Old Linux impl defined to be 400.
 */
#define IGMP_AGE_THRESHOLD		540	

/*
 * IGMPv3 default variables
 */
#define IGMP_INIT_ROBVAR	2	/* Default [Robustness variable] */
#define IGMP_MAX_ROBVAR		7	/* Max. [Robustness variable] */
#define IGMP_INIT_QRYINT	125	/* Default [Querier's Query interval] */
#define IGMP_MAX_QRYINT		255	/* Max. [Querier's Query interval] */
#define IGMP_INIT_QRYRSP	10	/* Default [Query Response interval] */
#define IGMP_DEF_QRYMRT		10	/* version 1 [Max. Response Time] */
#define IGMP_UNSOL_INT		1	/* [Unsolicited Report Interval] */

/* 
 * IGMPv3 report type(s)
 */
#define IGMP_REPORT_MODE_IN	    1	/* mode-is-include */
#define IGMP_REPORT_MODE_EX	    2	/* mode-is-exclude */
#define IGMP_REPORT_TO_IN	    3	/* change-to-include */
#define IGMP_REPORT_TO_EX	    4	/* change-to-exclude */
#define IGMP_REPORT_ALLOW_NEW	5	/* allow-new-sources */
#define IGMP_REPORT_BLOCK_OLD	6	/* block-old-sources */

/*
 * Report type as flagged bits in reporttag
 */
#define IGMP_MASK_CUR_STATE	0x01	/* report current-state */
#define IGMP_MASK_ALLOW_NEW	0x02	/* report source as allow-new */
#define IGMP_MASK_BLOCK_OLD	0x04	/* report source as block-old */
#define IGMP_MASK_TO_IN		0x08	/* report source as to_in */
#define IGMP_MASK_TO_EX		0x10	/* report source as to_ex */
#define IGMP_MASK_STATE_T1	0x20	/* state at T1 */
#define IGMP_MASK_STATE_T2	0x40	/* state at T2 */
#define IGMP_MASK_IF_STATE	0x80	/* report current-state per interface */

#define IGMP_MASK_STATE_TX	(IGMP_MASK_STATE_T1 | IGMP_MASK_STATE_T2)
#define IGMP_MASK_PENDING	(IGMP_MASK_CUR_STATE | IGMP_MASK_ALLOW_NEW | \
                                 IGMP_MASK_BLOCK_OLD)

/*
 * List identifiers
 */
#define IGMP_EXCLUDE_LIST	1	/* exclude list used to tag report */
#define IGMP_INCLUDE_LIST	2	/* include list used to tag report */
#define IGMP_RECORDED_LIST	3	/* recorded list used to tag report */

/*
 * This information is logically part of the in_device structure.
 */
struct router_info 
{
	struct in_device 	*rti_ifp;
	int			    rti_type;	/* host igmp compat mode */	
	int 			rti_timev1;	/* [IGMPv1 querier present] */
	int			    rti_timev2;	/* [IGMPv2 querier present] */
	int 			rti_timer;	/* Report to general query */
	int 			rti_qrv;	/* [Querier Robustness Var] */
	struct router_info	*rti_next;
	int		        rti_refcnt;
};

/*
 * Internet multicast source filter list.  This list is used to store ip
 * source addresses, per interface membership information.
 */
struct in_sfentry 
{
	struct in_addr		isf_addr;	    /* the sourcefilter address */
 	unsigned short		isf_refcount;	/* references by sockets */
	unsigned short		isf_reporttag;	/* tracks what to report */
	unsigned short		isf_rexmit;     /* retrans state/count */
	struct in_sfentry	*isf_next;	    /* next filter */
	struct in_sfentry	*isf_pnext;	    /* next in pool (for re-use) */
};	

/*
 * Internet multicast address structure.  There is one of these for each IP
 * multicast group to which this host belongs on a given network interface.
 * For every entry on the interface's if_multiaddrs list which represents an
 * IP multicast group, there is one of these structures.  They are also kept on
 * a system-wide list to make it easier to keep our legacy IGMP code compatible
 * with the rest of the world. 
 *
 * XXX How significant is this last bit w.r.t. Linux?
 */
struct in_multi 
{
    /* Pointers to maintain the global Multicast address list */
	struct 
    {
	    struct in_multi *le_next;
	    struct in_multi **le_prev;
	} inm_link; 

    /* Next Multicast Group on the per-interface list */
	struct in_multi			*inm_next;

	struct in_addr			inm_addr;   /* Multicast Group Address */
	struct in_device		*inm_ifp;   /* Interface IP structure */

    /* 
     * Per-interface IGMP Router information 
     * Has Per-Interface timer for scheduling responses to General Query 
     */
	struct router_info		*inm_rti;

    /* 
     * Per-Group and Per-Interface timers for scheduling responses
     * to Group-Specific and Group-And-Source-Specific Queries
     * and State-Change Reports
     */
    /* Group-Specific Report timer */
	int				        inm_timer;
    /* Group-And-Source-Specific report timer */
	int				        inm_ti_curstate;
    /* State-Change Report timer */
	int				        inm_ti_statechg;

	int				        inm_ti_statechg_interval;

    /* 
     * Number of pending State-Change Reports to be sent 
     * Set to Querier Robustness Value
     */
    unsigned short			inm_rpt_statechg;

    /* 
     * Number of pending Filter-Mode-Change Reports to be sent 
     * Set to Querier Robustness Value
     * If this is non-zero, it means a Filter-Mode-Change Report
     * is to be transmitted next even if any Source List changes
     * schedule any Source-List-Change Reports
     */
   	unsigned short			inm_rpt_toxx;

    /*
     * Initial value is IGMP_NONEXISTENT
     * It is set to IGMP_OTHERMEMBER if we have last heard
     * a report from another member, or to IGMP_IREPORTEDLAST 
     * if we were the last to send a report
     */
	unsigned int			inm_state;	

    /* Interface Filter Mode */
	unsigned short			inm_fmode;

    /* 
     * Reference count of sockets in Exclude mode 
     * associted with this interface
     */
	unsigned int			inm_num_socks_excl;
    
    /* Source Filter list - Exclude/Include modes */
	struct in_sfentry		*inm_sflexcl;
	struct in_sfentry		*inm_sflincl;

    /* Source Filters recorded from a Group-And-Source-Specific Query */
	struct in_sfentry		*inm_sflrec;

    /* Free (unused) Source Filter entry pool */
	struct in_sfentry		*inm_sflpool;

    /* 
     * Number of Source Filters recorded for a pending response to
     * a Group-And-Source-Specific Query
     */
	unsigned int			inm_num_rec_sources;

    /*
     * Number of Group-And-Source-Specific Queries to be accepted
     * while a response (GASS Report) is pending to a 
     * Group-And-Source-Specific Query
     */
	unsigned int			inm_num_gass_queries;

	/* Extra stuff to mimic BSD ifmultiaddr struct */
	unsigned int			users;
	unsigned int			loaded;	

#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
    int                     inm_startup_statechg_report;
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */

    int                     inm_grp_self;

    unsigned long           inm_last_report_time;
    unsigned int            inm_start_total_stats;
    unsigned int            inm_total_joins;
    unsigned int            inm_total_leaves;
};

/*
 * Structure used by macros below to remember position when stepping through
 * all of the in_multi records.
 */
struct in_multistep {
	struct in_multi *i_inm;
};

struct source_list {
	struct source_list 	*src_next;		/* next entry */
	struct in_addr	   	src_addr;		/* address */
};

struct sock_mcastsf {
	unsigned int	   	smsf_fmode;		    /* source filter mode */
	unsigned int	   	smsf_numsrc; 		/* number of sources */
	struct source_list 	*smsf_slist;		/* addresses */
};


extern struct proc_dir_entry *igmp_proc_dir;

struct in_multi *in_addmulti(struct in_addr *ap, struct in_device *ifp);
void in_delmulti(struct in_addr *ap, struct in_device *ifp);

int igmp_rcv(struct sk_buff *skb);
int ip_check_mc(struct in_device *in_dev, u32 mc_addr);
void igmp_init(void);

void ip_mc_inc_group(struct in_device *in_dev, u32 addr);
int ip_mc_dec_group(struct in_device *in_dev, u32 addr);

void ip_mc_down(struct in_device *in_dev);
void ip_mc_up(struct in_device *in_dev);
void ip_mc_destroy_dev(struct in_device *in_dev);

void ip_mc_drop_socket(struct sock *sk);

int ip_setmoptions(struct sock *sk, int optname, char *optval, int optlen);
int in_setmcast_srcfilter(struct sock *sk, unsigned long arg);
int in_getmcast_srcfilter(struct sock *sk, unsigned long arg);


/*
 *  IGMPv3 locking functions.  The idea is that we want to allow recursive
 *  calls from same process (e.g., ip_setmoptions() -> ip_addmembership() -> 
 *  ip_route_output() -> ip_route_output_slow() -> ip_check_mc()).  So, we
 *  store the process id after grabbing the lock and clear it before releasing
 *  the lock.  We count levels of recursion and only release the lock after
 *  the outermost unlock() call.
 */  
extern spinlock_t igmp_spinlock;  
extern pid_t igmp_spinlock_pid;
extern unsigned igmp_spinlock_level;
static inline void igmp_lock(void)
{
	if (igmp_spinlock_pid != current->pid) 
    {
		spin_lock_bh(&igmp_spinlock);	
		igmp_spinlock_pid = current->pid;
	} 
	igmp_spinlock_level++;
}

static inline void igmp_unlock(void)
{	
	if (igmp_spinlock_pid == current->pid) 
    {
		if (--igmp_spinlock_level == 0) 
        {
			igmp_spinlock_pid = -1;
			spin_unlock_bh(&igmp_spinlock);
		}
	} 
    else 
    {
		// printk(KERN_CRIT "igmp_unlock: unlocking unheld lock!\n");
		BUG();
	}
}


#if defined(CONFIG_RG_IGMP_PROXY) || defined (CONFIG_RG_IGMP_PROXY_MODULE)
void igmp_print_inmp(struct in_multi *inmp);

struct in_multi *igmp_add_membership(struct in_device *in_dev, unsigned int group);
void igmp_remove_membership(struct in_device *in_dev, unsigned int group);
int igmp_update_membership(struct in_device *in_dev, unsigned int group,
                           struct source_list *src_rem, int fmode_from, 
                           struct source_list *src_add, int fmode_to);
void igmp_update_last_report_time(struct in_device *in_dev, unsigned int group);
void igmp_update_total_joins(struct in_device *in_dev, unsigned int group);
void igmp_update_total_leaves(struct in_device *in_dev, unsigned int group);
#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
void igmp_update_startup_statechg_report(struct in_device *in_dev, unsigned int group);
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */
void igmp_inc_group_self(struct in_device *in_dev, unsigned int group);
void igmp_dec_group_self(struct in_device *in_dev, unsigned int group);
unsigned int igmp_get_inm_users(struct in_device *in_dev, unsigned int group);

void igmp_set_client_unsol_interval(int unsol_interval);
int igmp_get_client_unsol_interval(void);
#ifdef ACTION_TEC_IGMP_PERSISTENT_JOIN
void igmp_set_persistent_join_interval(int persis_join_interval);
int igmp_get_persistent_join_interval(void);
#endif /* ACTION_TEC_IGMP_PERSISTENT_JOIN */
int igmp_get_client_version(void);

int igmp_host_max_group_table_size(void);
int igmp_host_group_table_size(struct in_device *in_dev);
int igmp_host_get_group_table_next_index(struct in_device *in_dev, int index);
int igmp_host_get_group_table_entry(struct in_device *in_dev, 
                                    struct igmp_host_group_req_t *grp_entry);
int igmp_host_get_gui_group_table_entries(struct in_device *in_dev, 
                                          struct igmp_host_gui_group_req_t *req);

int igmp_host_groupstats_table_size(struct in_device *in_dev);
int igmp_host_get_groupstats_table_next_index(struct in_device *in_dev, int index);
int igmp_host_get_groupstats_table_entry(struct in_device *in_dev, 
                                         struct igmp_host_groupstats_req_t *grp_entry);
int igmp_host_get_gui_groupstat_table_entries(struct in_device *in_dev, 
                                          struct igmp_host_gui_groupstat_req_t *req);
#endif /* defined(CONFIG_RG_IGMP_PROXY) || defined (CONFIG_RG_IGMP_PROXY_MODULE) */

#endif /* __KERNEL__ */

#else

/*
 *	Linux NET3:	Internet Group Management Protocol  [IGMP]
 *
 *	Authors:
 *		Alan Cox <Alan.Cox@linux.org>
 *
 *	Extended to talk the BSD extended IGMP protocol of mrouted 3.6
 *
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

/*
 *	IGMP protocol structures
 */

/*
 *	Header in on cable format
 */

struct igmphdr
{
	__u8 type;
	__u8 code;		/* For newer IGMP */
	__u16 csum;
	__u32 group;
};

#define IGMP_HOST_MEMBERSHIP_QUERY	0x11	/* From RFC1112 */
#define IGMP_HOST_MEMBERSHIP_REPORT	0x12	/* Ditto */
#define IGMP_DVMRP			0x13	/* DVMRP routing */
#define IGMP_PIM			0x14	/* PIM routing */
#define IGMP_TRACE			0x15
#define IGMP_HOST_NEW_MEMBERSHIP_REPORT 0x16	/* New version of 0x11 */
#define IGMP_HOST_LEAVE_MESSAGE 	0x17

#define IGMP_MTRACE_RESP		0x1e
#define IGMP_MTRACE			0x1f


/*
 *	Use the BSD names for these for compatibility
 */

#define IGMP_DELAYING_MEMBER		0x01
#define IGMP_IDLE_MEMBER		0x02
#define IGMP_LAZY_MEMBER		0x03
#define IGMP_SLEEPING_MEMBER		0x04
#define IGMP_AWAKENING_MEMBER		0x05

#define IGMP_MINLEN			8

#define IGMP_MAX_HOST_REPORT_DELAY	10	/* max delay for response to */
						/* query (in seconds)	*/

#define IGMP_TIMER_SCALE		10	/* denotes that the igmphdr->timer field */
						/* specifies time in 10th of seconds	 */

#define IGMP_AGE_THRESHOLD		400	/* If this host don't hear any IGMP V1	*/
						/* message in this period of time,	*/
						/* revert to IGMP v2 router.		*/

#define IGMP_ALL_HOSTS		htonl(0xE0000001L)
#define IGMP_ALL_ROUTER 	htonl(0xE0000002L)
#define IGMP_LOCAL_GROUP	htonl(0xE0000000L)
#define IGMP_LOCAL_GROUP_MASK	htonl(0xFFFFFF00L)

/*
 * struct for keeping the multicast list in
 */

#ifdef __KERNEL__

/* ip_mc_socklist is real list now. Speed is not argument;
   this list never used in fast path code
 */

struct ip_mc_socklist
{
	struct ip_mc_socklist	*next;
	int			count;
	struct ip_mreqn		multi;
};

struct ip_mc_list
{
	struct in_device	*interface;
	unsigned long		multiaddr;
	struct ip_mc_list	*next;
	struct timer_list	timer;
	int			users;
	atomic_t		refcnt;
	spinlock_t		lock;
	char			tm_running;
	char			reporter;
	char			unsolicit_count;
	char			loaded;
};

extern int ip_check_mc(struct in_device *dev, u32 mc_addr);
extern int igmp_rcv(struct sk_buff *);
extern int ip_mc_join_group(struct sock *sk, struct ip_mreqn *imr);
extern int ip_mc_leave_group(struct sock *sk, struct ip_mreqn *imr);
extern void ip_mc_drop_socket(struct sock *sk);
extern void ip_mr_init(void);
extern void ip_mc_init_dev(struct in_device *);
extern void ip_mc_destroy_dev(struct in_device *);
extern void ip_mc_up(struct in_device *);
extern void ip_mc_down(struct in_device *);
extern void ip_mc_dec_group(struct in_device *in_dev, u32 addr);
extern void ip_mc_inc_group(struct in_device *in_dev, u32 addr);
#endif

#endif /* ACTION_TEC_IGMPV3 */

#endif /* _LINUX_IGMP_H */

