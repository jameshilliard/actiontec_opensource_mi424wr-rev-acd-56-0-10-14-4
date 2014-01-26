#ifndef _FTAPE_BUFFER_H
#define _FTAPE_BUFFER_H

/*
 *      Copyright (C) 1997 Claus-Justus Heine.

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
 * $Source: /home/cvsroot/bhr-4.0.16/rg/os/linux-2.4/drivers/char/ftape/lowlevel/ftape-buffer.h,v $
 * $Revision: 1.1.1.1 $
 * $Date: 2006/01/20 03:16:21 $
 *
 *  This file contains the allocator/dealloctor for ftape's dynamic dma
 *  buffer.
 */

extern int  ftape_set_nr_buffers(int cnt);

#endif
