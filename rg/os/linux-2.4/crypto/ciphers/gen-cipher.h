/*
   Generic cipher template header

   $Id: gen-cipher.h,v 1.1.1.1 2006/01/20 03:16:22 zjjiang Exp $

   Written by Herbert Valerio Riedel <hvr@gnu.org> 

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software

   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#if !defined(CIPHER_KEY_SIZE_MASK)
# error CIPHER_KEY_SIZE_MASK not defined
#endif

#if !defined(CIPHER_ID)
# error CIPHER_ID not defined
#endif

#if !defined(CIPHER_BLOCKSIZE)
#error CIPHER_BLOCKSIZE not defined
#endif

#if !defined(CIPHER_KEY_SCHEDULE_SIZE)
#error CIPHER_KEY_SCHEDULE_SIZE not defined
#endif

/* some cpp magic */
#define __STR(x) # x
#define __xSTR(x) __STR(x)

#define __CAT(x,y) x ## y
#define __xCAT(x,y) __CAT(x,y)

#define CIPHER_SYMBOL(x) __xCAT(CIPHER_ID,x)

#if !defined(CIPHER_STR)
# define CIPHER_STR __xSTR(CIPHER_ID)
#endif

/* default lock ops */

#if !defined(NO_CIPHER_DEFAULT_LOCK_OPS)
static void
CIPHER_SYMBOL(_lock) (void)
{
  MOD_INC_USE_COUNT;
}

static void
CIPHER_SYMBOL(_unlock) (void)
{
  MOD_DEC_USE_COUNT;
}
#endif /* NO_CIPHER_DEFAULT_LOCK_OPS */

/*
 * compat #defines for gen-{ecb,cbc,cfb,ctr,rtc}.h
 */

#define CIPHER_NAME(x) CIPHER_SYMBOL(x)

#if CIPHER_BLOCKSIZE == 64
# define CIPHER_BITS_64
#elif CIPHER_BLOCKSIZE == 128
# define CIPHER_BITS_128
#else
# error unsupported cipher blocksize
#endif


#define __xINIT_CIPHER_BLKOPS(name)    INIT_CIPHER_BLKOPS(name)
#define __xINIT_CIPHER_OPS(name)       INIT_CIPHER_OPS(name)

/* ECB */

#include "gen-ecb.h"

static struct cipher_implementation CIPHER_SYMBOL(_ecb) = {
  {{NULL, NULL}, CIPHER_MODE_ECB, CIPHER_STR "-ecb"},
  blocksize: CIPHER_BLOCKSIZE / 8,
  ivsize: 0,
  key_schedule_size: CIPHER_KEY_SCHEDULE_SIZE,
  key_size_mask: CIPHER_KEY_SIZE_MASK,
  __xINIT_CIPHER_BLKOPS (CIPHER_SYMBOL(_ecb)),
  __xINIT_CIPHER_OPS (CIPHER_ID)
};

/* CBC */

#include "gen-cbc.h"

static struct cipher_implementation CIPHER_SYMBOL(_cbc) = {
  {{NULL, NULL}, CIPHER_MODE_CBC, CIPHER_STR "-cbc"},
  blocksize: CIPHER_BLOCKSIZE / 8,
  ivsize: CIPHER_BLOCKSIZE / 8,
  key_schedule_size: CIPHER_KEY_SCHEDULE_SIZE,
  key_size_mask: CIPHER_KEY_SIZE_MASK,
  __xINIT_CIPHER_BLKOPS (CIPHER_SYMBOL(_cbc)),
  __xINIT_CIPHER_OPS (CIPHER_ID)
};

/* CFB */

#include "gen-cfb.h"

static struct cipher_implementation CIPHER_SYMBOL(_cfb) = {
  {{NULL, NULL}, CIPHER_MODE_CFB, CIPHER_STR "-cfb"},
  blocksize: CIPHER_BLOCKSIZE / 8,
  ivsize: CIPHER_BLOCKSIZE / 8,
  key_schedule_size: CIPHER_KEY_SCHEDULE_SIZE,
  key_size_mask: CIPHER_KEY_SIZE_MASK,
  __xINIT_CIPHER_BLKOPS (CIPHER_SYMBOL(_cfb)),
  __xINIT_CIPHER_OPS (CIPHER_ID)
};

/* CTR */

#include "gen-ctr.h"

static struct cipher_implementation CIPHER_SYMBOL(_ctr) = {
  {{NULL, NULL}, CIPHER_MODE_CTR, CIPHER_STR "-ctr"},
  blocksize: CIPHER_BLOCKSIZE / 8,
  ivsize: CIPHER_BLOCKSIZE / 8,
  key_schedule_size: CIPHER_KEY_SCHEDULE_SIZE,
  key_size_mask: CIPHER_KEY_SIZE_MASK,
  __xINIT_CIPHER_BLKOPS (CIPHER_SYMBOL(_ctr)),
  __xINIT_CIPHER_OPS (CIPHER_ID)
};

/* RTC */

#include "gen-rtc.h"

static struct cipher_implementation CIPHER_SYMBOL(_rtc) = {
  {{NULL, NULL}, CIPHER_MODE_RTC, CIPHER_STR "-rtc"},
  blocksize: CIPHER_BLOCKSIZE / 8,
  ivsize: CIPHER_BLOCKSIZE / 8,
  key_schedule_size: CIPHER_KEY_SCHEDULE_SIZE,
  key_size_mask: CIPHER_KEY_SIZE_MASK,
  __xINIT_CIPHER_BLKOPS (CIPHER_SYMBOL(_rtc)),
  __xINIT_CIPHER_OPS (CIPHER_ID)
};

/* default init/cleanup functions */

#if !defined(NO_CIPHER_DEFAULT_INIT_CLEANUP_OPS)

static int __init
CIPHER_SYMBOL (_init) (void)
{
#if defined(CIPHER_PRE_INIT_HOOK)
  CIPHER_PRE_INIT_HOOK
#endif

  if (register_cipher (& CIPHER_SYMBOL(_ecb)))
    printk (KERN_WARNING "Couldn't register " CIPHER_STR "-ecb encryption\n");

  if (register_cipher (& CIPHER_SYMBOL(_cbc)))
    printk (KERN_WARNING "Couldn't register " CIPHER_STR "-cbc encryption\n");

  if (register_cipher (& CIPHER_SYMBOL(_cfb)))
    printk (KERN_WARNING "Couldn't register " CIPHER_STR "-cfb encryption\n");

  if (register_cipher (& CIPHER_SYMBOL(_ctr)))
    printk (KERN_WARNING "Couldn't register " CIPHER_STR "-ctr encryption\n");

  if (register_cipher (& CIPHER_SYMBOL(_rtc)))
    printk (KERN_WARNING "Couldn't register " CIPHER_STR "-rtc encryption\n");

  return 0;
}

#define __xmodule_init(s) module_init(s)
__xmodule_init (CIPHER_SYMBOL (_init));
#undef __xmodule_init

static void __exit
CIPHER_SYMBOL (_cleanup) (void)
{
  if (unregister_cipher (& CIPHER_SYMBOL(_ecb)))
    printk (KERN_WARNING "Couldn't unregister " CIPHER_STR "-ecb encryption\n");

  if (unregister_cipher (& CIPHER_SYMBOL(_cbc)))
    printk (KERN_WARNING "Couldn't unregister " CIPHER_STR "-cbc encryption\n");

  if (unregister_cipher (& CIPHER_SYMBOL(_cfb)))
    printk (KERN_WARNING "Couldn't unregister " CIPHER_STR "-cfb encryption\n");

  if (unregister_cipher (& CIPHER_SYMBOL(_ctr)))
    printk (KERN_WARNING "Couldn't unregister " CIPHER_STR "-ctr encryption\n");

  if (unregister_cipher (& CIPHER_SYMBOL(_rtc)))
    printk (KERN_WARNING "Couldn't unregister " CIPHER_STR "-rtc encryption\n");
}

#define __xmodule_exit(s) module_exit(s)
__xmodule_exit (CIPHER_SYMBOL (_cleanup));
#undef __xmodule_exit

#endif /* NO_CIPHER_DEFAULT_INIT_CLEANUP_OPS */

/* EOF */
