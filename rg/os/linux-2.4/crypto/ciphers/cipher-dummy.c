/*
 * Dummy CIPHER module
 *
 * this is a dummy cipher module for debugging purposes, it may serve as 
 * skeleton for new ciphers as well
 *
 * by hvr@gnu.org
*/

#include <asm/byteorder.h>
#include <linux/crypto.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/string.h>

#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif

static int
dummy_encrypt (struct cipher_context *cx, const u8 *in8, u8 *out8,
	       int size, int atomic)
{
	memmove (out8, in8, size);
	return 0;
}

static int
dummy_decrypt (struct cipher_context *cx, const u8 *in8, u8 *out8,
	       int size, int atomic)
{
	memmove (out8, in8, size);
	return 0;
}

static int
dummy_set_key (struct cipher_context *cx, const unsigned char *key, int key_len,
	       int atomic)
{
	printk (KERN_DEBUG "%s: key_len=%d atomic=%d\n",
		__PRETTY_FUNCTION__, key_len, atomic);

	if (key_len != 16 && key_len != 20 && key_len != 24 && key_len != 32)
		return -EINVAL;	/* unsupported key length */

	cx->key_length = key_len;

	return 0;
}

#define CIPHER_ID                dummy
#define CIPHER_BLOCKSIZE         64
#define CIPHER_KEY_SIZE_MASK     CIPHER_KEYSIZE_64 
#define CIPHER_KEY_SCHEDULE_SIZE ((18+1024)*sizeof(u32))

#include "gen-cipher.h"

EXPORT_NO_SYMBOLS;
