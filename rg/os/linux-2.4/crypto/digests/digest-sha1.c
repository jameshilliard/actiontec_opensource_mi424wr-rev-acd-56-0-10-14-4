/* $Id: digest-sha1.c,v 1.1.1.1 2006/01/20 03:16:22 zjjiang Exp $
 *
 * Modified by Andrew McDonald <andrew@mcdonald.org.uk> from md5glue
 * by Alan Smithee, mailed to maintainer on pulped trees.
 *
 * Contains SHA-1 code from Steve Reid, licensed in the public domain.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/crypto.h>

#include <asm/uaccess.h>
#include <asm/byteorder.h>

#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif /* MODULE_LICENSE */
#ifdef MODULE_DESCRIPTION
MODULE_DESCRIPTION ("SHA1 Digest / CryptoAPI");
#endif

typedef struct {
	u64 count;
	u32 state[5];
	u8 buffer[64];
} sha1_ctx_t;

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
# define blk0(i) block32[i]

#define blk(i) (block32[i&15] = rol(block32[(i+13)&15]^block32[(i+8)&15] \
    ^block32[(i+2)&15]^block32[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5); \
                        w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5); \
                        w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5); \
                        w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);


/* Hash a single 512-bit block. This is the core of the algorithm. */
static void
SHA1Transform(u32 state[5], const u8 buffer[64])
{
	register u32 a, b, c, d, e;

	u32 block32[16];

	/* convert/copy data to workspace */
	for (a = 0; a < sizeof(block32)/sizeof(u32); a++)
	  block32[a] = be32_to_cpu (((const u32 *)buffer)[a]);

	/* Copy context->state[] to working vars */
	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];

	/* 4 rounds of 20 operations each. Loop unrolled. */
	R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
	R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
	R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
	R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
	R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
	R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
	R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
	R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
	R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
	R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
	R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
	R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
	R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
	R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
	R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
	R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
	R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
	R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
	R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
	R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);
	/* Add the working vars back into context.state[] */
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
	/* Wipe variables */
	a = b = c = d = e = 0;
	memset (block32, 0x00, sizeof block32);
}

/* SHA1Init - Initialize new context */
static inline void
SHA1Init(sha1_ctx_t* ctx)
{
	/* SHA1 initialization constants */
	const static sha1_ctx_t initstate = {
	  0,
	  { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 },
	  { 0, }
	};

	*ctx = initstate;
}


/* Run your data through this. */
static void
SHA1Update(sha1_ctx_t* ctx, const u8* data, unsigned len)
{
	unsigned i, j;

	j = (ctx->count >> 3) & 0x3f;

	ctx->count += len << 3;

	if ((j + len) > 63) {
		memcpy(&ctx->buffer[j], data, (i = 64-j));
		SHA1Transform(ctx->state, ctx->buffer);
		for ( ; i + 63 < len; i += 64) {
			SHA1Transform(ctx->state, &data[i]);
		}
		j = 0;
	}
	else i = 0;
	memcpy(&ctx->buffer[j], &data[i], len - i);
}


/* Add padding and return the message digest. */
static void
SHA1Final(sha1_ctx_t* ctx, u8 digest[20])
{
	const static u8 padding[64] = { 0x80, };
	u32 i, j, index, padLen;
	u64 t;
	u8 bits[8] = { 0, };

	t = ctx->count;
	bits[7] = 0xff & t; t>>=8;
	bits[6] = 0xff & t; t>>=8;
	bits[5] = 0xff & t; t>>=8;
	bits[4] = 0xff & t; t>>=8;
	bits[3] = 0xff & t; t>>=8;
	bits[2] = 0xff & t; t>>=8;
	bits[1] = 0xff & t; t>>=8;
	bits[0] = 0xff & t;

	/* Pad out to 56 mod 64 */
	index = (ctx->count >> 3) & 0x3f;
	padLen = (index < 56) ? (56 - index) : ((64+56) - index);
	SHA1Update(ctx, padding, padLen);

	/* Append length */
	SHA1Update(ctx, bits, sizeof bits); 

	/* Store state in digest */
	for (i = j = 0; i < 5; i++, j += 4) {
		u32 t2 = ctx->state[i];
		digest[j+3] = t2 & 0xff; t2>>=8;
		digest[j+2] = t2 & 0xff; t2>>=8;
		digest[j+1] = t2 & 0xff; t2>>=8;
		digest[j  ] = t2 & 0xff;
	}

	/* Wipe context */
	memset(ctx, 0, sizeof *ctx);
}

/*
 *
 */

static int
sha1_open(struct digest_context *cx, int atomic)
{
	sha1_ctx_t *const ctx = (sha1_ctx_t *) cx->digest_info;

	SHA1Init (ctx);

	return 0;
}

static int
sha1_update(struct digest_context *cx, const u8 *in, int size, int atomic)
{
	sha1_ctx_t *const ctx = (sha1_ctx_t *) cx->digest_info;

	SHA1Update (ctx, in, size);

	return 0;
}

static int
sha1_digest(struct digest_context *cx, u8 *out, int atomic)
{
	sha1_ctx_t *const ctx = (sha1_ctx_t *) cx->digest_info;

	sha1_ctx_t *const ctx_tmp = kmalloc (sizeof *ctx_tmp, GFP_KERNEL);

	if (!ctx_tmp)
		return -ENOMEM;

	*ctx_tmp = *ctx;

	SHA1Final (ctx_tmp, out);

	kfree (ctx_tmp);

	return 0;
}

static int
sha1_close(struct digest_context *cx, u8 *out, int atomic)
{
	sha1_ctx_t *const ctx = (sha1_ctx_t *) cx->digest_info;

	static u8 tmp[20];

	SHA1Final(ctx, out ? out : tmp);

	return 0;
}

#define DIGEST_ID		sha1
#define DIGEST_BLOCKSIZE	20

#include "gen-hash.h"

EXPORT_NO_SYMBOLS;
