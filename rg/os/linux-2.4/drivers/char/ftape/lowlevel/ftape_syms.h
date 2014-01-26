#ifndef _FTAPE_SYMS_H
#define _FTAPE_SYMS_H

/*
 * Copyright (C) 1996-1997 Claus-Justus Heine

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *
 * $Source: /home/cvsroot/bhr-4.0.16/rg/os/linux-2.4/drivers/char/ftape/lowlevel/ftape_syms.h,v $
 * $Revision: 1.1.1.1 $
 * $Date: 2006/01/20 03:16:21 $
 *
 *      This file contains the definitions needed to export symbols
 *      from the QIC-40/80/3010/3020 floppy-tape driver "ftape" for
 *      Linux.
 */

#if LINUX_VERSION_CODE < KERNEL_VER(2,1,18)
#include <linux/module.h>
/*      ftape-vfs.c defined global vars.
 */

extern struct symbol_table ftape_symbol_table;
#endif

#endif
