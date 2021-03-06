/* Machine-dependent pthreads configuration and inline functions.
   i386 version.
   Copyright (C) 1996-2001, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Richard Henderson <rth@tamu.edu>.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef _PT_MACHINE_H
#define _PT_MACHINE_H	1

#ifndef __ASSEMBLER__
#ifndef PT_EI
# define PT_EI extern inline
#endif

extern long int testandset (int *spinlock);
extern int __compare_and_swap (long int *p, long int oldval, long int newval);

/* Get some notion of the current stack.  Need not be exactly the top
   of the stack, just something somewhere in the current frame.  */
#define CURRENT_STACK_FRAME  __builtin_frame_address (0)


/* See if we can optimize for newer cpus... */
#if defined __GNUC__ && __GNUC__ >= 2 && defined __i486__ || defined __pentium__ || defined __pentiumpro__

/* Spinlock implementation; required.  */
PT_EI long int
testandset (int *spinlock)
{
  long int ret;

  __asm__ __volatile__ (
	"xchgl %0, %1"
	: "=r" (ret), "=m" (*spinlock)
	: "0" (1), "m" (*spinlock)
	: "memory");

  return ret;
}

/* B22020: The following #define has been #undef'ed because using
 * compare-and-swap as implemented in x86 assembly language causes some client
 * code to crash. */

/* Compare-and-swap for semaphores.  It's always available on i686.  */
#undef HAS_COMPARE_AND_SWAP

PT_EI int
__compare_and_swap (long int *p, long int oldval, long int newval)
{
  char ret;
  long int readval;

  __asm__ __volatile__ ("lock; cmpxchgl %3, %1; sete %0"
			: "=q" (ret), "=m" (*p), "=a" (readval)
			: "r" (newval), "m" (*p), "a" (oldval)
			: "memory");
  return ret;
}

#if __ASSUME_LDT_WORKS > 0
#include "../useldt.h"
#endif

/* The P4 and above really want some help to prevent overheating.  */
#define BUSY_WAIT_NOP	__asm__ ("rep; nop")


#else /* Generic i386 implementation */



/* Spinlock implementation; required.  */
PT_EI long int
testandset (int *spinlock)
{
  long int ret;

  __asm__ __volatile__(
       "xchgl %0, %1"
       : "=r"(ret), "=m"(*spinlock)
       : "0"(1), "m"(*spinlock)
       : "memory");

  return ret;
}


/* B22020: The following #defines have been #undef'ed because using
 * compare-and-swap as implemented in x86 assembly language causes some client
 * code to crash. */

/* Compare-and-swap for semaphores.
   Available on the 486 and above, but not on the 386.
   We test dynamically whether it's available or not. */

#undef HAS_COMPARE_AND_SWAP
#undef TEST_FOR_COMPARE_AND_SWAP

PT_EI int
__compare_and_swap (long int *p, long int oldval, long int newval)
{
  char ret;
  long int readval;

  __asm__ __volatile__ ("lock; cmpxchgl %3, %1; sete %0"
			: "=q" (ret), "=m" (*p), "=a" (readval)
			: "r" (newval), "m" (*p), "a" (oldval)
			: "memory");
  return ret;
}


PT_EI int
get_eflags (void)
{
  int res;
  __asm__ __volatile__ ("pushfl; popl %0" : "=r" (res) : );
  return res;
}


PT_EI void
set_eflags (int newflags)
{
  __asm__ __volatile__ ("pushl %0; popfl" : : "r" (newflags) : "cc");
}


PT_EI int
compare_and_swap_is_available (void)
{
  int oldflags = get_eflags ();
  int changed;
  /* Flip AC bit in EFLAGS.  */
  set_eflags (oldflags ^ 0x40000);
  /* See if bit changed.  */
  changed = (get_eflags () ^ oldflags) & 0x40000;
  /* Restore EFLAGS.  */
  set_eflags (oldflags);
  /* If the AC flag did not change, it's a 386 and it lacks cmpxchg.
     Otherwise, it's a 486 or above and it has cmpxchg.  */
  return changed != 0;
}
#endif /* Generic i386 implementation */

#endif /* __ASSEMBLER__ */

#endif /* pt-machine.h */
