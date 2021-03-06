/*
 * ftruncate64 syscall.  Copes with 64 bit and 32 bit machines
 * and on 32 bit machines this sends things into the kernel as
 * two 32-bit arguments (high and low 32 bits of length) that 
 * are ordered based on endianess.  It turns out endian.h has
 * just the macro we need to order things, __LONG_LONG_PAIR.
 *
 *  Copyright (C) 2002  Erik Andersen <andersen@codepoet.org>
 *
 * This file is subject to the terms and conditions of the GNU
 * Lesser General Public License.  See the file COPYING.LIB in
 * the main directory of this archive for more details.
 */

#include <features.h>
#include <unistd.h>
#include <errno.h>
#include <endian.h>
#include <stdint.h>
#include <sys/syscall.h>

#if defined __NR_ftruncate64

#if __WORDSIZE == 64 || (defined(__powerpc__) && defined (__UCLIBC_HAS_LFS__))

/* For a 64 bit machine, life is simple... */
_syscall2(int, ftruncate64, int, fd, __off64_t, length);

#elif __WORDSIZE == 32

#if defined __UCLIBC_HAS_LFS__

#ifndef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...) __syscall_ftruncate64 (args)
#define __NR___syscall_ftruncate64 __NR_ftruncate64
static inline _syscall3(int, __syscall_ftruncate64, int, fd, int, high_length, int, low_length);
#endif


/* The exported ftruncate64 function.  */
int ftruncate64 (int fd, __off64_t length)
{
    uint32_t low = length & 0xffffffff;
    uint32_t high = length >> 32;
    return INLINE_SYSCALL(ftruncate64, 3, fd, __LONG_LONG_PAIR (high, low));
}
#endif /* __UCLIBC_HAS_LFS__ */

#else /* __WORDSIZE */
#error Your machine is not 64 bit or 32 bit, I am dazed and confused.
#endif /* __WORDSIZE */

#endif

