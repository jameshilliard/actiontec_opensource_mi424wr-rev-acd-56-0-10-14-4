#ifndef _IGMPV3_PROXY_CFG_H
#define _IGMPV3_PROXY_CFG_H

#include <linux/config.h>

#ifdef ACTION_TEC_IGMPV3

#include <net/if.h>
#include "igmpv3_proxy.h"

typedef struct igmp_params_t
{
    igmp_wan_if_t *wans;
    igmp_lan_if_t *lans;

    int client_unsol_rpt_interval;

    int qcm;
    int fastleave;
    int robustness;
    int query_interval;
    int query_resp_interval;
    int startup_query_interval;
    int startup_query_count;
    int lastmember_query_interval;
    int lastmember_query_count;
} igmp_params_t;


int igmp_init(igmp_t *igmp);
int igmp_uninit(igmp_t *igmp);

int igmp_wan_add(igmp_t *igmp, char *if_name);
int igmp_wan_del(igmp_t *igmp, char *if_name);

int igmp_lan_add(igmp_t *igmp, char *if_name);
int igmp_lan_del(igmp_t *igmp, char *if_name);

int igmp_get_status(igmp_t *igmp);

#if 0
int igmpv3_host_get_gui_group_entry(igmp_t *igmp, 
                                    struct igmp_host_gui_group_req_t *req);
int igmpv3_host_get_gui_groupstat_entry(igmp_t *igmp, 
                                    struct igmp_host_gui_groupstat_req_t *req);
#endif

int igmp_counters_reconf(igmp_t *igmp, igmp_params_t *params);
int igmp_proxy_mcf_reconf(igmp_t *igmp, struct igmprx_mcfgrp_req *req);
int igmp_reconf(igmp_t *igmp, igmp_params_t *params);

igmp_t *igmp_open(void);
void igmp_close(igmp_t *igmp);

#endif /* ACTION_TEC_IGMPV3 */

#endif /* _IGMPV3_PROXY_CFG_H */
