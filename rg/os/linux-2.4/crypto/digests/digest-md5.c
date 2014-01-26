/*
 * MD5 digest implementation / cryptoapi
 *
 * This code implements the MD5 message-digest algorithm.  The
 * algorithm is due to Ron Rivest.  This implementation is based upon
 * the public domain implementation written by Colin Plumb in 1993.
 *
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

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/string.h>

#include <linux/crypto.h>

#include <asm/uaccess.h>
#include <asm/byteorder.h>

#ifdef MODULE_LICENSE
MODULE_LICENSE ("GPL");
#endif
#ifdef MODULE_DESCRIPTION
MODULE_DESCRIPTION ("MD5 Digest Implementation / CryptoAPI");
#endif

typedef struct {
	u64 byte_count;
	u32 hash[4];		/* hash buf */
	u32 in[16];		/* 64-byte inbuffer */
} md5_ctx_t;

/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f,w,x,y,z,in,s) \
	 (w += f(x,y,z) + in, w = (w<<s | w>>(32-s)) + x)

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static inline void
md5_transform (u32 hash[4], u32 const in[16])
{
	register u32 a, b, c, d;

	a = hash[0];
	b = hash[1];
	c = hash[2];
	d = hash[3];

	MD5STEP (F1, a, b, c, d, in[0] + 0xd76aa478, 7);
	MD5STEP (F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
	MD5STEP (F1, c, d, a, b, in[2] + 0x242070db, 17);
	MD5STEP (F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
	MD5STEP (F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
	MD5STEP (F1, d, a, b, c, in[5] + 0x4787c62a, 12);
	MD5STEP (F1, c, d, a, b, in[6] + 0xa8304613, 17);
	MD5STEP (F1, b, c, d, a, in[7] + 0xfd469501, 22);
	MD5STEP (F1, a, b, c, d, in[8] + 0x698098d8, 7);
	MD5STEP (F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
	MD5STEP (F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
	MD5STEP (F1, b, c, d, a, in[11] + 0x895cd7be, 22);
	MD5STEP (F1, a, b, c, d, in[12] + 0x6b901122, 7);
	MD5STEP (F1, d, a, b, c, in[13] + 0xfd987193, 12);
	MD5STEP (F1, c, d, a, b, in[14] + 0xa679438e, 17);
	MD5STEP (F1, b, c, d, a, in[15] + 0x49b40821, 22);

	MD5STEP (F2, a, b, c, d, in[1] + 0xf61e2562, 5);
	MD5STEP (F2, d, a, b, c, in[6] + 0xc040b340, 9);
	MD5STEP (F2, c, d, a, b, in[11] + 0x265e5a51, 14);
	MD5STEP (F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
	MD5STEP (F2, a, b, c, d, in[5] + 0xd62f105d, 5);
	MD5STEP (F2, d, a, b, c, in[10] + 0x02441453, 9);
	MD5STEP (F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
	MD5STEP (F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
	MD5STEP (F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
	MD5STEP (F2, d, a, b, c, in[14] + 0xc33707d6, 9);
	MD5STEP (F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
	MD5STEP (F2, b, c, d, a, in[8] + 0x455a14ed, 20);
	MD5STEP (F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
	MD5STEP (F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
	MD5STEP (F2, c, d, a, b, in[7] + 0x676f02d9, 14);
	MD5STEP (F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

	MD5STEP (F3, a, b, c, d, in[5] + 0xfffa3942, 4);
	MD5STEP (F3, d, a, b, c, in[8] + 0x8771f681, 11);
	MD5STEP (F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
	MD5STEP (F3, b, c, d, a, in[14] + 0xfde5380c, 23);
	MD5STEP (F3, a, b, c, d, in[1] + 0xa4beea44, 4);
	MD5STEP (F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
	MD5STEP (F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
	MD5STEP (F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
	MD5STEP (F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
	MD5STEP (F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
	MD5STEP (F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
	MD5STEP (F3, b, c, d, a, in[6] + 0x04881d05, 23);
	MD5STEP (F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
	MD5STEP (F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
	MD5STEP (F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
	MD5STEP (F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

	MD5STEP (F4, a, b, c, d, in[0] + 0xf4292244, 6);
	MD5STEP (F4, d, a, b, c, in[7] + 0x432aff97, 10);
	MD5STEP (F4, c, d, a, b, in[14] + 0xab9423a7, 15);
	MD5STEP (F4, b, c, d, a, in[5] + 0xfc93a039, 21);
	MD5STEP (F4, a, b, c, d, in[12] + 0x655b59c3, 6);
	MD5STEP (F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
	MD5STEP (F4, c, d, a, b, in[10] + 0xffeff47d, 15);
	MD5STEP (F4, b, c, d, a, in[1] + 0x85845dd1, 21);
	MD5STEP (F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
	MD5STEP (F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
	MD5STEP (F4, c, d, a, b, in[6] + 0xa3014314, 15);
	MD5STEP (F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
	MD5STEP (F4, a, b, c, d, in[4] + 0xf7537e82, 6);
	MD5STEP (F4, d, a, b, c, in[11] + 0xbd3af235, 10);
	MD5STEP (F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
	MD5STEP (F4, b, c, d, a, in[9] + 0xeb86d391, 21);

	hash[0] += a;
	hash[1] += b;
	hash[2] += c;
	hash[3] += d;
}

static inline void
le32_to_cpu_array (u32 *buf, unsigned words)
{
	while (words--) {
		__le32_to_cpus (buf);
		buf++;
	}
}

static inline void
cpu_to_le32_array (u32 *buf, unsigned words)
{
	while (words--) {
		__cpu_to_le32s (buf);
		buf++;
	}
}

static inline void
md5_transform_helper (md5_ctx_t * ctx)
{
	le32_to_cpu_array (ctx->in, sizeof (ctx->in) / sizeof (u32));
	md5_transform (ctx->hash, ctx->in);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern 
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
static inline void
md5_final (md5_ctx_t * ctx, u8 out[16])
{
	/* Number of bytes in ctx->in */
	const int offset = ctx->byte_count & 0x3f;

	/* p points after last content byte in ctx->in */
	u8 *p = (u8 *) ctx->in + offset;

	/* Bytes of padding needed to make 56 bytes (-8..55) */
	int padding = 56 - (offset + 1);

	/* Set the first byte of padding to 0x80.  There is always room. */
	*p++ = 0x80;

	if (padding < 0) {	/* Padding forces an extra block */
		memset (p, 0x00, padding + sizeof (u64));

		md5_transform_helper (ctx);

		p = (u8 *) ctx->in;
		padding = 56;
	}

	/* pad remaining bytes w/ 0x00 */
	memset (p, 0x00, padding);

	/* Append length in bits and transform */
	ctx->in[14] = ctx->byte_count << 3;	/* low order word first */
	ctx->in[15] = ctx->byte_count >> 29;

	/* keep the appended bit-count words in host order! */
	le32_to_cpu_array (ctx->in,
			   (sizeof (ctx->in) - sizeof (u64)) / sizeof (u32));
	md5_transform (ctx->hash, ctx->in);

	/* convert digest buf from host to LE byteorder */
	cpu_to_le32_array (ctx->hash, sizeof (ctx->hash) / sizeof (u32));

	/* copy to output buffer */
	memcpy (out, ctx->hash, sizeof (ctx->hash));

	/* wipe context */
	memset (ctx, 0, sizeof (ctx));
}

/*****************************************************************************/
/* public entry points */

/*
 * Initialize MD5 context.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */

static int
md5_open (struct digest_context *cx, int atomic)
{
	md5_ctx_t *ctx = (md5_ctx_t *) cx->digest_info;

	ctx->hash[0] = 0x67452301;
	ctx->hash[1] = 0xefcdab89;
	ctx->hash[2] = 0x98badcfe;
	ctx->hash[3] = 0x10325476;

	ctx->byte_count = 0;

	return 0;
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */

static int
md5_update (struct digest_context *cx, const u8 *in, int size, int atomic)
{
	md5_ctx_t *ctx = (md5_ctx_t *) cx->digest_info;

	/* Space available in ctx->in (at least 1) */
	const u32 avail = sizeof (ctx->in) - (ctx->byte_count & 0x3f);

	/* Update byte count */
	ctx->byte_count += size;

	/* if in fits in ctx->in just copy and return */
	if (avail > size) {
		memcpy ((u8 *) ctx->in + (sizeof (ctx->in) - avail), in, size);
		return 0;
	}

	/* First chunk is an odd size */
	memcpy ((u8 *) ctx->in + (sizeof (ctx->in) - avail), in, avail);

	md5_transform_helper (ctx);

	in += avail;
	size -= avail;

	/* Process data in sizeof(ctx->in) chunks */
	while (size >= sizeof (ctx->in)) {
		memcpy (ctx->in, in, sizeof (ctx->in));

		md5_transform_helper (ctx);

		in += sizeof (ctx->in);
		size -= sizeof (ctx->in);
	}

	/* assert (size < sizeof(ctx->in)); */

	/* Handle any remaining bytes of data. */
	memcpy (ctx->in, in, size);

	return 0;
}

static int
md5_close (struct digest_context *cx, u8 *out, int atomic)
{
	md5_ctx_t *ctx = (md5_ctx_t *) cx->digest_info;

	md5_final (ctx, out);

	return 0;
}

static int
md5_digest (struct digest_context *cx, u8 *out, int atomic)
{
	md5_ctx_t *ctx = (md5_ctx_t *) cx->digest_info;
	md5_ctx_t *ctx_copy;

	/* work on copy */
	ctx_copy = kmalloc (sizeof (md5_ctx_t), GFP_KERNEL);
	memcpy (ctx_copy, ctx, sizeof (md5_ctx_t));

	md5_final (ctx_copy, out);

	kfree (ctx_copy);

	return 0;
}

#define DIGEST_ID		md5
#define DIGEST_BLOCKSIZE	16 /* sizeof (md5_ctx_t.hash) */

#include "gen-hash.h"

EXPORT_NO_SYMBOLS;
