/* $Id: gen-hmac.h,v 1.1.1.1 2006/01/20 03:16:22 zjjiang Exp $ */
/*
 * Copyright (C)2002 USAGI/WIDE Project
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/types.h>
#include <linux/string.h>

/*
 * Supported(Tested) algorithms are MD5 and SHA1.
 * Digest size is 64 bytes. 
 * (See RFC2401)
 *
 * This code derived from sample code in RFC2104.
 */

#define HMAC_BLOCK_SIZE 64

int 
DIGEST_SYMBOL(_hmac)(struct digest_context *cx, const u8 *key, int key_len, const u8 *in, int size, u8 *hmac, int atomic)
{
	int i;
	int error = 0;
	const int blocksize = cx->di->blocksize;
	u8 k_ipad[HMAC_BLOCK_SIZE+1];    /* inner padding - key XORd with ipad */
	u8 k_opad[HMAC_BLOCK_SIZE+1];    /* outer padding - key XORd with opad */

	/* If key is longer than 64 bytes, reset it to key=H(key) */
	if (key_len > HMAC_BLOCK_SIZE) {

		u8 *tk = kmalloc(blocksize, GFP_KERNEL);
		if (!tk) {
			printk(KERN_ERR "%s: tk is not allocated\n", __FUNCTION__);
			error = -ENOMEM;
			goto err;
		}

		memset(tk, 0, blocksize);

		DIGEST_SYMBOL(_open)(cx, atomic);
		DIGEST_SYMBOL(_update)(cx, key, key_len, atomic);
		DIGEST_SYMBOL(_close)(cx, tk, atomic);
		key = tk;
		key_len = blocksize;

		kfree(tk);
	}

	/*
	 * the HMAC_MD5 transform looks like:
	 *
	 * H(K XOR opad, H(K XOR ipad, text))
	 *
	 * where K is an n byte key
	 * ipad is the byte 0x36 repeated 64 times
	 * opad is the byte 0x5c repeated 64 times
	 * and text is the data being protected
	 */

	/* start out by storing key in pads */
	memset(k_ipad,0,sizeof(k_ipad));
	memset(k_opad,0,sizeof(k_opad));
	memcpy(k_ipad,key,key_len);
	memcpy(k_opad,key,key_len);

	/* XOR key with ipad and opad values */
	for (i=0; i<HMAC_BLOCK_SIZE; i++) {
		k_ipad[i] ^= 0x36;
		k_opad[i] ^= 0x5c;
	}

	/* perform inner */
	DIGEST_SYMBOL(_open)(cx, atomic);
	DIGEST_SYMBOL(_update)(cx, k_ipad, HMAC_BLOCK_SIZE, atomic);
	DIGEST_SYMBOL(_update)(cx, in, size, atomic);
	DIGEST_SYMBOL(_close)(cx, hmac, atomic);

	/* perform outer */
	DIGEST_SYMBOL(_open)(cx, atomic);
	DIGEST_SYMBOL(_update)(cx, k_opad, HMAC_BLOCK_SIZE, atomic);
	DIGEST_SYMBOL(_update)(cx, hmac, blocksize, atomic);
	DIGEST_SYMBOL(_close)(cx, hmac, atomic);

err:
	return error;
}

