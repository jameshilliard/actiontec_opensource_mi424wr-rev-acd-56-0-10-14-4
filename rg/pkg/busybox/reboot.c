/* vi: set sw=4 ts=4: */
/*
 * Mini reboot implementation for busybox
 *
 *
 * Copyright (C) 1995, 1996 by Bruce Perens <bruce@pixar.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include "busybox.h"
#include <signal.h>
#ifndef CONFIG_OPENRG
  #include <rg_error_native.h>
#else
  #include <misc_funcs.h>
#endif


extern int reboot_main(int argc, char **argv)
{
#ifndef CONFIG_OPENRG
    rg_error(LINFO, "The system is going DOWN for reboot.");
#else
    rg_error(LINFO, SYSLOG_SYS_UPDOWN_FMT "The system is going DOWN for "
	"reboot.");
#endif
#ifdef BB_FEATURE_LINUXRC
    /* don't assume init's pid == 1 */
    return(kill(*(find_pid_by_name("init")), SIGINT));
#else
    return(kill(1, SIGINT));
#endif
}

/*
Local Variables:
c-file-style: "linux"
c-basic-offset: 4
tab-width: 4
End:
*/
