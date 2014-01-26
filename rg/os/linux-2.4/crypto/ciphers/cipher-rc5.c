/* $Id: cipher-rc5.c,v 1.1.1.1 2006/01/20 03:16:22 zjjiang Exp $
 *
 * rc5.c				RC5-32/[12,16]/b
 *
 * Copyright (c) 1999 Pekka Riikonen <priikone@poseidon.pspt.fi>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, dis-
 * tribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the fol-
 * lowing conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABIL-
 * ITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABIL-
 * ITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of the authors shall
 * not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization from
 * the authors.
 * 
 * This cipher is patented, and is free for educational usage. For
 * commercial use, please contact RSA Security, Inc.
 * 
 */

#include <linux/module.h>
#include <linux/errno.h> 
#include <linux/init.h>  
#include <linux/string.h> 

#include <linux/crypto.h> 
#include <linux/wordops.h> 

#ifdef MODULE_LICENSE
MODULE_LICENSE("Dual BSD/GPL");
#endif
#ifdef MODULE_DESCRIPTION
MODULE_DESCRIPTION ("RC5 Cipher / CryptoAPI");
#endif
#ifdef MODULE_AUTHOR
MODULE_AUTHOR ("Pekka Riikonen <priikone@poseidon.pspt.fi>");
#endif

/* 12 rounds for testing, 16 rounds for use */
#ifdef CRYPTOAPI_TEST
	#define ROUNDS 12
#else
	#define ROUNDS 16
#endif /* CRYPTOAPI_TEST */

/* RC5 definitions */
#define W	32	/* word size, in bits */
#define MIN_KEY	16	/* minimum key size in bytes */
#define C	8	/* same for 128,  192 and 256 bits key */
#define TABSIZE	(2 *(ROUNDS + 1)) /* size of S table, t = 2 * (r + 1) */

#define rotl generic_rotl32
#define rotr generic_rotr32

/* RC5 encryption */
#define RC5E(i, A, B, S) do {                   \
                A = A ^ B;                      \
                A = rotl(A, B) + S[i];          \
                B = B ^ A;                      \
                B = rotl(B, A) + S[i + 1];      \
		} while(0);
 
/* RC5 decryption */
#define RC5D(i, A, B, S) do {                   \
                B = B - S[i + 1];               \
                B = rotr(B, A) ^ A;             \
                A = A - S[i];                   \
                A = rotr(A, B) ^ B;             \
                } while(0);

struct rc5_ctx {
	u32 xk[TABSIZE];
};

/* Sets RC5 key */
static int
rc5_set_key(struct cipher_context *cx, const u8 *key, int key_len,
	    int atomic)
{
	struct rc5_ctx *ctx;
	const u32 *in_key = (const u32 *)key;
	u32 i, j, k, A, B, L[C], c;

	ctx = (struct rc5_ctx *) cx->keyinfo;

 	if (key_len != 16 && key_len != 24 && key_len != 32)
		return -EINVAL; /* unsupported key length */

	cx->key_length = key_len;

	key_len *= 8;
	c = key_len / W;

	/* init L */
	for (i = 0; i < C; i++) {
		L[i] = 0;
	}

	for (i = 0; i < (key_len / W); i++) {
		L[i] = le32_to_cpu (in_key[i]);
	}

	/* initialize expanded key array */
	ctx->xk[0] = 0xb7e15163;
	for (i = 1; i < TABSIZE; i++) {
		ctx->xk[i] = ctx->xk[i - 1] + 0x9e3779b9;
	}

	/* mix L and key array (S) */
	A = B = 0;
	i = j = 0;
	for (k = 0; k < (3 * TABSIZE); k++) {
		A = ctx->xk[i] = rotl(ctx->xk[i] + A + B, 3);
		B = L[j] = rotl(L[j] + (A + B), A + B);
		i = (i + 1) % TABSIZE;
		j = (j + 1) % c;
	}

	return 0;
}

/* Encrypts *one* block at a time. */
static int
rc5_encrypt(struct cipher_context *cx, const u8 *in8,
	    u8 *out8, int size, int atomic)
{
	const struct rc5_ctx *ctx = (struct rc5_ctx *) cx->keyinfo;

	const u32 *in = (const u32 *)in8;
	u32 *out = (u32 *)out8;

	u32 A = le32_to_cpu (in[0]) + ctx->xk[0];
	u32 B = le32_to_cpu (in[1]) + ctx->xk[1];

	int i;

	for (i = 1; i <= ROUNDS; i++) {
		RC5E(i * 2, A, B, ctx->xk);
	}

	out[0] = cpu_to_le32 (A);
	out[1] = cpu_to_le32 (B);

	return 0;
}

/* Decrypts *one* block at a time. */
static int
rc5_decrypt(struct cipher_context *cx, const u8 *in8,
	    u8 *out8, int size, int atomic)
{
	const struct rc5_ctx *ctx = (struct rc5_ctx *) cx->keyinfo;

	const u32 *in = (u32 *)in8;
	u32 *out = (u32 *)out8;

	u32 A = le32_to_cpu (in[0]);
	u32 B = le32_to_cpu (in[1]);

	int i;

	for (i = ROUNDS; i >= 1; i--) {
		RC5D(i * 2, A, B, ctx->xk);
	}

	out[0] = cpu_to_le32 (A - ctx->xk[0]);
	out[1] = cpu_to_le32 (B - ctx->xk[1]);

	return 0;
}   

#define CIPHER_ID                rc5
#define CIPHER_BLOCKSIZE         64
#define CIPHER_KEY_SIZE_MASK     CIPHER_KEYSIZE_128 | CIPHER_KEYSIZE_192 | \
                                 CIPHER_KEYSIZE_256
#define CIPHER_KEY_SCHEDULE_SIZE sizeof(struct rc5_ctx)

#include "gen-cipher.h"

EXPORT_NO_SYMBOLS;

/* eof */
