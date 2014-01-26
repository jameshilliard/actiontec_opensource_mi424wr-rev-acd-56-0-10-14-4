/*
 * RFC2367 PF_KEYv2 Key management API message parser
 * Copyright (C) 1999  Richard Guy Briggs.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * RCSID $Id: pfkey_v2_ext_bits.c,v 1.1.1.1 2006/01/20 03:16:23 zjjiang Exp $
 */

/*
 *		Template from klips/net/ipsec/ipsec/ipsec_parse.c.
 */

char pfkey_v2_ext_bits_c_version[] = "$USAGI: pfkey_v2_ext_bits.c,v 1.1 2001/12/23 16:01:12 mk Exp $";

/*
 * Some ugly stuff to allow consistent debugging code for use in the
 * kernel and in user space
*/

#ifdef __KERNEL__

#include <linux/kernel.h>  /* for printk */

#include <linux/slab.h> /* kmalloc() */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/interrupt.h> /* mark_bh */

#include <linux/netdevice.h>   /* struct device, and other headers */
#include <linux/etherdevice.h> /* eth_type_trans */
#include <linux/ip.h>          /* struct iphdr */ 
#if defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE)
#include <linux/ipv6.h>
#endif /* defined(CONFIG_IPV6) || defined(CONFIG_IPV6_MODULE) */

#else /* __KERNEL__ */

#include <sys/types.h>
#include <linux/types.h>
#include <linux/errno.h>
#endif

#include <net/pfkeyv2.h>
#include <net/pfkey.h>

unsigned int extensions_bitmaps[2/*in/out*/][2/*perm/req*/][SADB_MAX + 1/*ext*/] = {

/* INBOUND EXTENSIONS */
{

/* PERMITTED IN */
{
/* SADB_RESERVED */
0
,
/* SADB_GETSPI PERMITTED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
	| 1<<SADB_EXT_SPIRANGE
,
/* SADB_UPDATE PERMITTED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_LIFETIME_CURRENT
	| 1<<SADB_EXT_LIFETIME_HARD
	| 1<<SADB_EXT_LIFETIME_SOFT
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
	| 1<<SADB_EXT_KEY_AUTH
	| 1<<SADB_EXT_KEY_ENCRYPT
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
,
/* SADB_ADD PERMITTED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_LIFETIME_HARD
	| 1<<SADB_EXT_LIFETIME_SOFT
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
	| 1<<SADB_EXT_KEY_AUTH
	| 1<<SADB_EXT_KEY_ENCRYPT
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
,
/* SADB_DELETE PERMITTED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
,
/* SADB_GET PERMITTED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
,
/* SADB_ACQUIRE PERMITTED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
	| 1<<SADB_EXT_PROPOSAL
,
/* SADB_REGISTER PERMITTED IN */
	1<<SADB_EXT_RESERVED
,
/* SADB_EXPIRE PERMITTED IN */
0
,
/* SADB_FLUSH PERMITTED IN */
	1<<SADB_EXT_RESERVED
,
/* SADB_DUMP PERMITTED IN */
	1<<SADB_EXT_RESERVED
,
/* SADB_X_PROMISC PERMITTED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_LIFETIME_CURRENT
	| 1<<SADB_EXT_LIFETIME_HARD
	| 1<<SADB_EXT_LIFETIME_SOFT
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
	| 1<<SADB_EXT_KEY_AUTH
	| 1<<SADB_EXT_KEY_ENCRYPT
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
	| 1<<SADB_EXT_PROPOSAL
	| 1<<SADB_EXT_SUPPORTED_AUTH
	| 1<<SADB_EXT_SUPPORTED_ENCRYPT
	| 1<<SADB_EXT_SPIRANGE
	| 1<<SADB_X_EXT_KMPRIVATE
	| 1<<SADB_X_EXT_SATYPE2
	| 1<<SADB_X_EXT_SA2
	| 1<<SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_PCHANGE PERMITTED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_LIFETIME_CURRENT
	| 1<<SADB_EXT_LIFETIME_HARD
	| 1<<SADB_EXT_LIFETIME_SOFT
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
	| 1<<SADB_EXT_KEY_AUTH
	| 1<<SADB_EXT_KEY_ENCRYPT
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
	| 1<<SADB_EXT_PROPOSAL
	| 1<<SADB_EXT_SUPPORTED_AUTH
	| 1<<SADB_EXT_SUPPORTED_ENCRYPT
	| 1<<SADB_EXT_SPIRANGE
	| 1<<SADB_X_EXT_KMPRIVATE
	| 1<<SADB_X_EXT_SATYPE2
	| 1<<SADB_X_EXT_SA2
	| 1<<SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_GRPSA PERMITTED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_X_EXT_SATYPE2
	| 1<<SADB_X_EXT_SA2
	| 1<<SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_ADDFLOW PERMITTED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_X_EXT_ADDRESS_SRC_FLOW
	| 1<<SADB_X_EXT_ADDRESS_DST_FLOW
#if 0
	| 1<<SADB_X_EXT_ADDRESS_SRC_MASK
	| 1<<SADB_X_EXT_ADDRESS_DST_MASK
#endif
,
/* SADB_X_DELFLOW PERMITTED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_X_EXT_ADDRESS_SRC_FLOW
	| 1<<SADB_X_EXT_ADDRESS_DST_FLOW
#if 0
	| 1<<SADB_X_EXT_ADDRESS_SRC_MASK
	| 1<<SADB_X_EXT_ADDRESS_DST_MASK
#endif
,
/* SADB_X_DEBUG PERMITTED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_X_EXT_DEBUG
},

/* REQUIRED IN */
{
/* SADB_RESERVED REQUIRED IN */
0
,
/* SADB_GETSPI REQUIRED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_SPIRANGE
,
/* SADB_UPDATE REQUIRED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_KEY_AUTH
	| 1<<SADB_EXT_KEY_ENCRYPT
,
/* SADB_ADD REQUIRED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	/*| 1<<SADB_EXT_KEY_AUTH*/
	/*| 1<<SADB_EXT_KEY_ENCRYPT*/
,
/* SADB_DELETE REQUIRED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
,
/* SADB_GET REQUIRED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
,
/* SADB_ACQUIRE REQUIRED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_PROPOSAL
,
/* SADB_REGISTER REQUIRED IN */
	1<<SADB_EXT_RESERVED
,
/* SADB_EXPIRE REQUIRED IN */
0
,
/* SADB_FLUSH REQUIRED IN */
	1<<SADB_EXT_RESERVED
,
/* SADB_DUMP REQUIRED IN */
	1<<SADB_EXT_RESERVED
,
/* SADB_X_PROMISC REQUIRED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_LIFETIME_CURRENT
	| 1<<SADB_EXT_LIFETIME_HARD
	| 1<<SADB_EXT_LIFETIME_SOFT
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
	| 1<<SADB_EXT_KEY_AUTH
	| 1<<SADB_EXT_KEY_ENCRYPT
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
	| 1<<SADB_EXT_PROPOSAL
	| 1<<SADB_EXT_SUPPORTED_AUTH
	| 1<<SADB_EXT_SUPPORTED_ENCRYPT
	| 1<<SADB_EXT_SPIRANGE
	| 1<<SADB_X_EXT_KMPRIVATE
	| 1<<SADB_X_EXT_SATYPE2
	| 1<<SADB_X_EXT_SA2
	| 1<<SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_PCHANGE REQUIRED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_LIFETIME_CURRENT
	| 1<<SADB_EXT_LIFETIME_HARD
	| 1<<SADB_EXT_LIFETIME_SOFT
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
	| 1<<SADB_EXT_KEY_AUTH
	| 1<<SADB_EXT_KEY_ENCRYPT
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
	| 1<<SADB_EXT_PROPOSAL
	| 1<<SADB_EXT_SUPPORTED_AUTH
	| 1<<SADB_EXT_SUPPORTED_ENCRYPT
	| 1<<SADB_EXT_SPIRANGE
	| 1<<SADB_X_EXT_KMPRIVATE
	| 1<<SADB_X_EXT_SATYPE2
	| 1<<SADB_X_EXT_SA2
	| 1<<SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_GRPSA REQUIRED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_DST
	/*| 1<<SADB_X_EXT_SATYPE2*/
	/*| 1<<SADB_X_EXT_SA2*/
	/*| 1<<SADB_X_EXT_ADDRESS_DST2*/
,
/* SADB_X_ADDFLOW REQUIRED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_X_EXT_ADDRESS_SRC_FLOW
	| 1<<SADB_X_EXT_ADDRESS_DST_FLOW
#if 0
	| 1<<SADB_X_EXT_ADDRESS_SRC_MASK
	| 1<<SADB_X_EXT_ADDRESS_DST_MASK
#endif
,
/* SADB_X_DELFLOW REQUIRED IN */
	1<<SADB_EXT_RESERVED
/*| 1<<SADB_EXT_SA*/
#if 0 /* SADB_X_CLREROUTE doesn't need all these... */
	| 1<<SADB_X_EXT_ADDRESS_SRC_FLOW
	| 1<<SADB_X_EXT_ADDRESS_DST_FLOW
#if 0
	| 1<<SADB_X_EXT_ADDRESS_SRC_MASK
	| 1<<SADB_X_EXT_ADDRESS_DST_MASK
#endif
#endif
,
/* SADB_X_DEBUG REQUIRED IN */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_X_EXT_DEBUG
}

},

/* OUTBOUND EXTENSIONS */
{

/* PERMITTED OUT */
{
/* SADB_RESERVED */
0
,
/* SADB_GETSPI PERMITTED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
,
/* SADB_UPDATE PERMITTED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_LIFETIME_CURRENT
	| 1<<SADB_EXT_LIFETIME_HARD
	| 1<<SADB_EXT_LIFETIME_SOFT
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
,
/* SADB_ADD PERMITTED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_LIFETIME_HARD
	| 1<<SADB_EXT_LIFETIME_SOFT
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
,
/* SADB_DELETE PERMITTED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
,
/* SADB_GET PERMITTED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
#if 0
	| 1<<SADB_EXT_LIFETIME_CURRENT
	| 1<<SADB_EXT_LIFETIME_HARD
	| 1<<SADB_EXT_LIFETIME_SOFT
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
#endif
	| 1<<SADB_EXT_KEY_AUTH
	| 1<<SADB_EXT_KEY_ENCRYPT
#if 0
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
#endif
,
/* SADB_ACQUIRE PERMITTED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
	| 1<<SADB_EXT_PROPOSAL
,
/* SADB_REGISTER PERMITTED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SUPPORTED_AUTH
	| 1<<SADB_EXT_SUPPORTED_ENCRYPT
,
/* SADB_EXPIRE PERMITTED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_LIFETIME_CURRENT
	| 1<<SADB_EXT_LIFETIME_HARD
	| 1<<SADB_EXT_LIFETIME_SOFT
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
,
/* SADB_FLUSH PERMITTED OUT */
	1<<SADB_EXT_RESERVED
,
/* SADB_DUMP PERMITTED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_LIFETIME_CURRENT
	| 1<<SADB_EXT_LIFETIME_HARD
	| 1<<SADB_EXT_LIFETIME_SOFT
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
	| 1<<SADB_EXT_KEY_AUTH
	| 1<<SADB_EXT_KEY_ENCRYPT
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
,
/* SADB_X_PROMISC PERMITTED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_LIFETIME_CURRENT
	| 1<<SADB_EXT_LIFETIME_HARD
	| 1<<SADB_EXT_LIFETIME_SOFT
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
	| 1<<SADB_EXT_KEY_AUTH
	| 1<<SADB_EXT_KEY_ENCRYPT
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
	| 1<<SADB_EXT_PROPOSAL
	| 1<<SADB_EXT_SUPPORTED_AUTH
	| 1<<SADB_EXT_SUPPORTED_ENCRYPT
	| 1<<SADB_EXT_SPIRANGE
	| 1<<SADB_X_EXT_KMPRIVATE
	| 1<<SADB_X_EXT_SATYPE2
	| 1<<SADB_X_EXT_SA2
	| 1<<SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_PCHANGE PERMITTED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_LIFETIME_CURRENT
	| 1<<SADB_EXT_LIFETIME_HARD
	| 1<<SADB_EXT_LIFETIME_SOFT
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
	| 1<<SADB_EXT_KEY_AUTH
	| 1<<SADB_EXT_KEY_ENCRYPT
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
	| 1<<SADB_EXT_PROPOSAL
	| 1<<SADB_EXT_SUPPORTED_AUTH
	| 1<<SADB_EXT_SUPPORTED_ENCRYPT
	| 1<<SADB_EXT_SPIRANGE
	| 1<<SADB_X_EXT_KMPRIVATE
	| 1<<SADB_X_EXT_SATYPE2
	| 1<<SADB_X_EXT_SA2
	| 1<<SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_GRPSA PERMITTED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_X_EXT_SATYPE2
	| 1<<SADB_X_EXT_SA2
	| 1<<SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_ADDFLOW PERMITTED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_X_EXT_ADDRESS_SRC_FLOW
	| 1<<SADB_X_EXT_ADDRESS_DST_FLOW
#if 0
	| 1<<SADB_X_EXT_ADDRESS_SRC_MASK
	| 1<<SADB_X_EXT_ADDRESS_DST_MASK
#endif
,
/* SADB_X_DELFLOW PERMITTED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_X_EXT_ADDRESS_SRC_FLOW
	| 1<<SADB_X_EXT_ADDRESS_DST_FLOW
#if 0
	| 1<<SADB_X_EXT_ADDRESS_SRC_MASK
	| 1<<SADB_X_EXT_ADDRESS_DST_MASK
#endif
,
/* SADB_X_DEBUG PERMITTED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_X_EXT_DEBUG
},

/* REQUIRED OUT */
{
/* SADB_RESERVED REQUIRED OUT */
0
,
/* SADB_GETSPI REQUIRED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
,
/* SADB_UPDATE REQUIRED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
,
/* SADB_ADD REQUIRED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
,
/* SADB_DELETE REQUIRED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
,
/* SADB_GET REQUIRED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_KEY_AUTH
	| 1<<SADB_EXT_KEY_ENCRYPT
,
/* SADB_ACQUIRE REQUIRED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_PROPOSAL
,
/* SADB_REGISTER REQUIRED OUT */
	1<<SADB_EXT_RESERVED
	/* | 1<<SADB_EXT_SUPPORTED_AUTH
	   | 1<<SADB_EXT_SUPPORTED_ENCRYPT */
,
/* SADB_EXPIRE REQUIRED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_LIFETIME_CURRENT
	/* | 1<<SADB_EXT_LIFETIME_HARD
	   | 1<<SADB_EXT_LIFETIME_SOFT */
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
,
/* SADB_FLUSH REQUIRED OUT */
	1<<SADB_EXT_RESERVED
,
/* SADB_DUMP REQUIRED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_KEY_AUTH
	| 1<<SADB_EXT_KEY_ENCRYPT
,
/* SADB_X_PROMISC REQUIRED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_LIFETIME_CURRENT
	| 1<<SADB_EXT_LIFETIME_HARD
	| 1<<SADB_EXT_LIFETIME_SOFT
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
	| 1<<SADB_EXT_KEY_AUTH
	| 1<<SADB_EXT_KEY_ENCRYPT
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
	| 1<<SADB_EXT_PROPOSAL
	| 1<<SADB_EXT_SUPPORTED_AUTH
	| 1<<SADB_EXT_SUPPORTED_ENCRYPT
	| 1<<SADB_EXT_SPIRANGE
	| 1<<SADB_X_EXT_KMPRIVATE
	| 1<<SADB_X_EXT_SATYPE2
	| 1<<SADB_X_EXT_SA2
	| 1<<SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_PCHANGE REQUIRED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_LIFETIME_CURRENT
	| 1<<SADB_EXT_LIFETIME_HARD
	| 1<<SADB_EXT_LIFETIME_SOFT
	| 1<<SADB_EXT_ADDRESS_SRC
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_EXT_ADDRESS_PROXY
	| 1<<SADB_EXT_KEY_AUTH
	| 1<<SADB_EXT_KEY_ENCRYPT
	| 1<<SADB_EXT_IDENTITY_SRC
	| 1<<SADB_EXT_IDENTITY_DST
	| 1<<SADB_EXT_SENSITIVITY
	| 1<<SADB_EXT_PROPOSAL
	| 1<<SADB_EXT_SUPPORTED_AUTH
	| 1<<SADB_EXT_SUPPORTED_ENCRYPT
	| 1<<SADB_EXT_SPIRANGE
	| 1<<SADB_X_EXT_KMPRIVATE
	| 1<<SADB_X_EXT_SATYPE2
	| 1<<SADB_X_EXT_SA2
	| 1<<SADB_X_EXT_ADDRESS_DST2
,
/* SADB_X_GRPSA REQUIRED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_DST
,
/* SADB_X_ADDFLOW REQUIRED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_EXT_SA
	| 1<<SADB_EXT_ADDRESS_DST
	| 1<<SADB_X_EXT_ADDRESS_SRC_FLOW
	| 1<<SADB_X_EXT_ADDRESS_DST_FLOW
#if 0
	| 1<<SADB_X_EXT_ADDRESS_SRC_MASK
	| 1<<SADB_X_EXT_ADDRESS_DST_MASK
#endif
,
/* SADB_X_DELFLOW REQUIRED OUT */
	1<<SADB_EXT_RESERVED
	/*| 1<<SADB_EXT_SA*/
	| 1<<SADB_X_EXT_ADDRESS_SRC_FLOW
	| 1<<SADB_X_EXT_ADDRESS_DST_FLOW
#if 0
	| 1<<SADB_X_EXT_ADDRESS_SRC_MASK
	| 1<<SADB_X_EXT_ADDRESS_DST_MASK
#endif
,
/* SADB_X_DEBUG REQUIRED OUT */
	1<<SADB_EXT_RESERVED
	| 1<<SADB_X_EXT_DEBUG
}
}
};

/*
 * $Log: pfkey_v2_ext_bits.c,v $
 * Revision 1.1.1.1  2006/01/20 03:16:23  zjjiang
 * initial release
 *
 * Revision 1.1  2003/12/21 16:06:47  doron
 * B9310 - Apply USAGI kernel patch for IPv6.
 * Patch is in /net/fs/arch/rg/customers/intel/ixp42x/ipv6/usagi-linux24-stable_20030214-2.4.20.diff.bz2
 *
 * Revision 1.2  2001/07/09 00:40:50  miyazawa
 * remove obsolete file
 *
 * Revision 1.7  2000/09/12 22:35:37  rgb
 * Restructured to remove unused extensions from CLEARFLOW messages.
 *
 * Revision 1.6  2000/09/09 06:39:01  rgb
 * Added comments for clarity.
 *
 * Revision 1.5  2000/06/02 22:54:14  rgb
 * Added Gerhard Gessler's struct sockaddr_storage mods for IPv6 support.
 *
 * Revision 1.4  2000/01/21 06:27:56  rgb
 * Added address cases for eroute flows.
 * Added comments for each message type.
 * Added klipsdebug switching capability.
 * Fixed GRPSA bitfields.
 *
 * Revision 1.3  1999/12/01 22:20:27  rgb
 * Remove requirement for a proxy address in an incoming getspi message.
 *
 * Revision 1.2  1999/11/27 11:57:06  rgb
 * Consolidated the 4 1-d extension bitmap arrays into one 4-d array.
 * Add CVS log entry to bottom of file.
 * Cleaned out unused bits.
 *
 */
