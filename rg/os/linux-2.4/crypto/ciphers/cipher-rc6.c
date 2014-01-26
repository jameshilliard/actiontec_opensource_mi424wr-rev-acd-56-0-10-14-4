/* NOTE: This implementation has been changed from the original
   source.  See ChangeLog for more information.
   Maintained by Alexander Kjeldaas <astor@fast.no>
 */

/* This is an independent implementation of the RC6 algorithm that	*/
/* Ron Rivest and RSA Labs have submitted as a candidate for the	*/
/* NIST AES activity.  Refer to RSA Labs and Ron Rivest for any 	*/
/* copyright, patent or license issues for the RC6 algorithm.		*/

/* Copyright in this implementation is held by Dr B R Gladman but	*/
/* I hereby give permission for its free direct or derivative use	*/
/* subject to acknowledgment of its origin and compliance with any	*/
/* constraints that are placed on the exploitation of RC6 by its	*/
/* designers.							        */

/* Dr Brian Gladman (gladman@seven77.demon.co.uk) 18th July 1998	*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/wordops.h>
#include <linux/crypto.h>

#ifdef MODULE_LICENSE
MODULE_LICENSE("Dual BSD/GPL");
#endif
#ifdef MODULE_DESCRIPTION
MODULE_DESCRIPTION ("RC6 Cipher / CryptoAPI");
#endif
#ifdef MODULE_AUTHOR
MODULE_AUTHOR ("Dr Brian Gladman <gladman@seven77.demon.co.uk>");
#endif

#if 0
#define rotl rotl32
#define rotr rotr32
#else
#define rotl generic_rotl32
#define rotr generic_rotr32
#endif

#define f_rnd(i,a,b,c,d)				   \
		u = rotl(d * (d + d + 1), 5);	   \
		t = rotl(b * (b + b + 1), 5);	   \
		a = rotl(a ^ t, u) + l_key[i];	   \
		c = rotl(c ^ u, t) + l_key[i + 1]

#define i_rnd(i,a,b,c,d)				   \
		u = rotl(d * (d + d + 1), 5);	   \
		t = rotl(b * (b + b + 1), 5);	   \
		c = rotr(c - l_key[i + 1], t) ^ u; \
		a = rotr(a - l_key[i], u) ^ t

/* initialise the key schedule from the user supplied key	*/

int rc6_set_key(struct cipher_context *cx, const unsigned char *key, int key_len,
		int atomic)
{       const u32 *in_key = (const u32 *)key;
        /* l_key - storage for the key schedule */
        u32 *l_key   = cx->keyinfo;
	u32	i, j, k, a, b, l[8], t;

	if (key_len != 16 && key_len != 24 && key_len != 32)
		return -EINVAL; /* unsupported key length */

	cx->key_length = key_len;

	key_len *= 8;

	l_key[0] = 0xb7e15163;

	for(k = 1; k < 44; ++k)
		l_key[k] = l_key[k - 1] + 0x9e3779b9;

	for(k = 0; k < key_len / 32; ++k)
		l[k] = le32_to_cpu (in_key[k]);

	t = (key_len / 32) - 1;

	a = b = i = j = 0;

	for(k = 0; k < 132; ++k)
	{	a = rotl(l_key[i] + a + b, 3); b += a;
		b = rotl(l[j] + b, b);
		l_key[i] = a; l[j] = b;
		i = (i == 43 ? 0 : i + 1);
		j = (j == t ? 0 : j + 1);
	}

	return 0;
};

/* encrypt a block of text	*/

int rc6_encrypt(struct cipher_context *cx, 
		const u8 *in, u8 *out, int size, int atomic)
{       const u32 *l_key = cx->keyinfo;
	const u32 *in_blk = (const u32 *)in;
	u32 *out_blk = (u32 *)out;
	u32 a,b,c,d,t,u;

	a = le32_to_cpu (in_blk[0]);
	b = le32_to_cpu (in_blk[1]) + l_key[0];
	c = le32_to_cpu (in_blk[2]);
	d = le32_to_cpu (in_blk[3]) + l_key[1];

	f_rnd( 2,a,b,c,d); f_rnd( 4,b,c,d,a);
	f_rnd( 6,c,d,a,b); f_rnd( 8,d,a,b,c);
	f_rnd(10,a,b,c,d); f_rnd(12,b,c,d,a);
	f_rnd(14,c,d,a,b); f_rnd(16,d,a,b,c);
	f_rnd(18,a,b,c,d); f_rnd(20,b,c,d,a);
	f_rnd(22,c,d,a,b); f_rnd(24,d,a,b,c);
	f_rnd(26,a,b,c,d); f_rnd(28,b,c,d,a);
	f_rnd(30,c,d,a,b); f_rnd(32,d,a,b,c);
	f_rnd(34,a,b,c,d); f_rnd(36,b,c,d,a);
	f_rnd(38,c,d,a,b); f_rnd(40,d,a,b,c);

	out_blk[0] = cpu_to_le32 (a + l_key[42]);
	out_blk[1] = cpu_to_le32 (b);
	out_blk[2] = cpu_to_le32 (c + l_key[43]);
	out_blk[3] = cpu_to_le32 (d);
	return 0;
};

/* decrypt a block of text	*/

int rc6_decrypt(struct cipher_context *cx, const u8 *in, u8 *out, int size,
		int atomic)
{       const u32 *l_key = cx->keyinfo;
	const u32 *in_blk = (const u32 *)in;
	u32 *out_blk = (u32 *)out;
	u32 a,b,c,d,t,u;

	d = le32_to_cpu (in_blk[3]);
	c = le32_to_cpu (in_blk[2]) - l_key[43]; 
	b = le32_to_cpu (in_blk[1]);
	a = le32_to_cpu (in_blk[0]) - l_key[42];

	i_rnd(40,d,a,b,c); i_rnd(38,c,d,a,b);
	i_rnd(36,b,c,d,a); i_rnd(34,a,b,c,d);
	i_rnd(32,d,a,b,c); i_rnd(30,c,d,a,b);
	i_rnd(28,b,c,d,a); i_rnd(26,a,b,c,d);
	i_rnd(24,d,a,b,c); i_rnd(22,c,d,a,b);
	i_rnd(20,b,c,d,a); i_rnd(18,a,b,c,d);
	i_rnd(16,d,a,b,c); i_rnd(14,c,d,a,b);
	i_rnd(12,b,c,d,a); i_rnd(10,a,b,c,d);
	i_rnd( 8,d,a,b,c); i_rnd( 6,c,d,a,b);
	i_rnd( 4,b,c,d,a); i_rnd( 2,a,b,c,d);

	out_blk[3] = cpu_to_le32 (d - l_key[1]);
	out_blk[2] = cpu_to_le32 (c); 
	out_blk[1] = cpu_to_le32 (b - l_key[0]);
	out_blk[0] = cpu_to_le32 (a); 
	return 0;
};

#define CIPHER_ID                rc6
#define CIPHER_BLOCKSIZE         128
#define CIPHER_KEY_SIZE_MASK     CIPHER_KEYSIZE_128 | CIPHER_KEYSIZE_192 | CIPHER_KEYSIZE_256
#define CIPHER_KEY_SCHEDULE_SIZE (44*sizeof(u32))

#include "gen-cipher.h"

EXPORT_NO_SYMBOLS;

/* eof */
