/****************************************************************************
 *
 * rg/vendor/actiontec/vi414wg/modules/latch.c
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

#include <linux/types.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/module.h>

#include <asm/hardware.h>
#include <asm/io.h>

#include "latch.h"

void vi414wg_latch_set(u8 line, u8 value)
{
    static volatile u16 *vi414wg_latch_base = NULL;
    static u16 vi414wg_latch_value = VI414WG_LATCH_DEFAULT;

    if (!vi414wg_latch_base)
    {
	*IXP425_EXP_CS1 = VI414WG_CS1_CONFIG;
	vi414wg_latch_base = ioremap(IXP425_EXP_BUS_CS1_BASE_PHYS, 2);
    }
	
    if (value)
	vi414wg_latch_value |= BIT(line);
    else
	vi414wg_latch_value &= ~BIT(line);
    
    *vi414wg_latch_base = vi414wg_latch_value;
}
EXPORT_SYMBOL(vi414wg_latch_set);

