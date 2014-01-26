/* 
 * $Id: cipher-null.c,v 1.1.1.1 2006/01/20 03:16:22 zjjiang Exp $
 * Null Function Cipher
 *
 * Copyright (C) 2002 Herbert Valerio Riedel <hvr@gnu.org>
 * 
 * This module is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This module is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this module; if not, write to the Free Software
 *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <asm/byteorder.h>
#include <linux/crypto.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/string.h>

#ifdef MODULE_LICENSE
MODULE_LICENSE ("GPL");
#endif
#ifdef MODULE_DESCRIPTION
MODULE_DESCRIPTION ("Null Function Cipher / CryptoAPI");
#endif
#ifdef MODULE_AUTHOR
MODULE_AUTHOR ("Herbert Valerio Riedel <hvr@gnu.org>");
#endif

struct null_key_t {
	/* empty */
};

static int
null_encrypt (struct cipher_context *cx, const u8 *in8, u8 *out8,
		  int size, int atomic)
{
	if (size != 16)
		return -EINVAL;

	memcpy (out8, in8, size);
	return 0;
}

static int
null_decrypt (struct cipher_context *cx, const u8 *in8, u8 *out8,
		  int size, int atomic)
{
	if (size != 16)
		return -EINVAL;

	memcpy (out8, in8, size);
	return 0;
}

static int
null_set_key (struct cipher_context *cx, const unsigned char *key,
		  int key_len, int atomic)
{
	if (key_len != 8 && key_len != 16 && key_len != 20 && key_len != 24
	    && key_len != 32)
		return -EINVAL;	/* unsupported key length */

	cx->key_length = key_len;

	return 0;
}

#define CIPHER_ID                null
#define CIPHER_BLOCKSIZE         128
#define CIPHER_KEY_SIZE_MASK     CIPHER_KEYSIZE_64 | CIPHER_KEYSIZE_128 | CIPHER_KEYSIZE_160 | CIPHER_KEYSIZE_192 | CIPHER_KEYSIZE_256
#define CIPHER_KEY_SCHEDULE_SIZE sizeof(struct null_key_t)

#include "gen-cipher.h"

EXPORT_NO_SYMBOLS;

/*
 * Overrides for Emacs so that we follow Linus's tabbing style.
 * Emacs will notice this stuff at the end of the file and automatically
 * adjust the settings for this buffer only.  This must remain at the end
 * of the file.
 * ---------------------------------------------------------------------------
 * Local variables:
 * c-indent-level: 8
 * c-brace-imaginary-offset: 0
 * c-brace-offset: -8
 * c-argdecl-indent: 8
 * c-label-offset: -8
 * c-continued-statement-offset: 8
 * c-continued-brace-offset: 0
 * End:
 */
