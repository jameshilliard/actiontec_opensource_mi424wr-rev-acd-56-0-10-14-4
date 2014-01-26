/* $Id: gen-hash.h,v 1.1.1.1 2006/01/20 03:16:22 zjjiang Exp $
 *
 * Generic template for adding hash algorithms to CryptoAPI.
 *
 * Copyright (c) 2002, Kyle McMartin <kyle@achilles.net>.
 * 
 * This source is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * at your option any later version.
 *
 */

#ifndef _GEN_HASH_H_
#define _GEN_HASH_H_

#if !defined(DIGEST_ID)
# error DIGEST_ID not defined
#endif


#if !defined(DIGEST_BLOCKSIZE)
# error DIGEST_BLOCKSIZE not defined
#endif

/* __x* defines needed for proper expansion of nested macros */
#define __STR(x) # x
#define __xSTR(x) __STR(x)

#define __CAT(x,y) x ## y
#define __xCAT(x,y) __CAT(x,y)

#define DIGEST_SYMBOL(x) __xCAT(DIGEST_ID,x)

#if !defined(DIGEST_STR)
# define DIGEST_STR __xSTR(DIGEST_ID)
#endif

#if !defined(DIGEST_CTX_TYPE)
# define DIGEST_CTX_TYPE DIGEST_SYMBOL(_ctx_t)
#endif

#if !defined(DIGEST_SIZE)
# define DIGEST_SIZE sizeof(DIGEST_CTX_TYPE)
#endif


/* */
#define __xINIT_DIGEST_OPS(name)       INIT_DIGEST_OPS(name)

#include "gen-hmac.h"

#if !defined(NO_DIGEST_DEFAULT_LOCK_OPS)
static void
DIGEST_SYMBOL(_lock)(void)
{
	MOD_INC_USE_COUNT;
}

static void
DIGEST_SYMBOL(_unlock)(void)
{
	MOD_DEC_USE_COUNT;
}
#endif /* NO_DIGEST_DEFAULT_LOCK_OPS */

static struct digest_implementation DIGEST_ID = {
	{{NULL, NULL}, 0, __xSTR(DIGEST_ID)},
	blocksize: DIGEST_BLOCKSIZE,
	working_size: DIGEST_SIZE,
	__xINIT_DIGEST_OPS(DIGEST_ID)
};

static int __init
DIGEST_SYMBOL(_init)(void)
{
	if (register_digest(& DIGEST_ID))
		printk (KERN_WARNING "Couldn't register " DIGEST_STR " digest\n");

	return 0;
}

#define __xmodule_init(s) module_init(s)
__xmodule_init(DIGEST_SYMBOL(_init));
#undef __xmodule_init

static void __exit
DIGEST_SYMBOL(_cleanup)(void)
{
	if (unregister_digest(& DIGEST_ID))
		printk (KERN_WARNING "Couldn't unregister " DIGEST_STR " digest\n");
}

#define __xmodule_exit(s) module_exit(s)
__xmodule_exit(DIGEST_SYMBOL(_cleanup));
#undef __xmodule_exit

#endif /* _GEN_HASH_H_ */
