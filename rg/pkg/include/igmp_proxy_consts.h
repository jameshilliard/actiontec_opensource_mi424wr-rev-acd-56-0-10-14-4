/****************************************************************************
 *
 * rg/pkg/include/igmp_proxy_consts.h
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
#ifndef _IGMP_PROXY_CONSTS_H_
#define _IGMP_PROXY_CONSTS_H_

#define IGMP_PROXY_BASE			220
#define IGMP_PROXY_INIT			(IGMP_PROXY_BASE + 0)
#define IGMP_PROXY_UNINIT	        (IGMP_PROXY_BASE + 1)
#define IGMP_PROXY_LAN_IF_ADD		(IGMP_PROXY_BASE + 2)
#define IGMP_PROXY_LAN_IF_DEL		(IGMP_PROXY_BASE + 3)
#define IGMP_PROXY_END                  (IGMP_PROXY_BASE + 3)

#endif

