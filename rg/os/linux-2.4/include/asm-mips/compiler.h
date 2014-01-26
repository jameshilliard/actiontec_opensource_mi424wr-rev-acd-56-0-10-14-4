/****************************************************************************
 *
 * rg/os/linux-2.4/include/asm-mips/compiler.h
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
#ifndef _ASM_COMPILER_H_
#define _ASM_COMPILER_H_

#include <linux/module.h>

/* This is used to fool ld's garbage collection (gc) mechanism.
 * MIPS implementation wraps 'static_unused' marked functions with 
 * save_static_function() code which is not function actually but some
 * prologue to the function it wraps. Because of this, there are no references
 * to a 'static_unused' marked function, and gc removed the code.
 * SAVE_UNUSED_FUNCTION() forces 'static_unused' symbol to be referenced.
 */

#define SAVE_UNUSED_FUNCTION(sym)			\
const char __kstrtab_##sym[]				\
__attribute__((section(".kstrtab"))) = #sym;		\
const struct module_symbol __ksymtab_##sym 		\
__attribute__((section("__ksymtab"))) =			\
{ (unsigned long)&sym, __kstrtab_##sym }

#endif
