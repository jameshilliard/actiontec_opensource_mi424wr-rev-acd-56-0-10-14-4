
#ifndef _IGMPV3_PROXY_WBM_H
#define _IGMPV3_PROXY_WBM_H

#include <linux/config.h>

#ifdef ACTION_TEC_IGMPV3

#include <web_mng/cgi/html_wbm.h>


void p_scr_igmp_mcgrp_summary(html_t **handle, void *param);
void s_scr_igmp_mcgrp_summary(void);

void p_scr_igmp_mcgrp_stats(html_t **handle, void *param);
void s_scr_igmp_mcgrp_stats(void);

void p_scr_igmp_proxy_config(html_t **handle, void *param);
void s_scr_igmp_proxy_config(void);

void igmp_proxy_wbm_init(void);
void igmp_proxy_wbm_uninit(void);

#endif /* ACTION_TEC_IGMPV3 */

#endif /* _IGMPV3_PROXY_WBM_H */
