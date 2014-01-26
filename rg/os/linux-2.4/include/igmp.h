/**************************************************************************** 
 *  Copyright (c) 2002 Jungo LTD. All Rights Reserved.
 * 
 *  /home/bat/bat/actiontec_bhr_4_0/rg/pkg/igmp/igmp.h
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

#ifndef _IGMP_H_
#define _IGMP_H_

#define OS_INCLUDE_NETIF
#include <os_includes.h>

#ifndef ACTION_TEC_IGMPV3

typedef struct igmp_t igmp_t;

typedef struct igmp_lan_if_t
{
    struct igmp_lan_if_t *next;
    char if_name[IFNAMSIZ];
} igmp_lan_if_t;

typedef struct igmp_params_t
{
    char default_if_name[IFNAMSIZ];
    igmp_lan_if_t *lans;
} igmp_params_t;

igmp_t *igmp_open(void);

int igmp_reconf(igmp_t *igmt, igmp_params_t *params);

void igmp_close(igmp_t *igmp);

#endif /* ACTION_TEC_IGMPV3 */

#endif

