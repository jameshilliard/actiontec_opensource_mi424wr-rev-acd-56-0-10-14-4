/*
 *    idea.c - C source code for IDEA block cipher.
 *      IDEA (International Data Encryption Algorithm), formerly known as
 *      IPES (Improved Proposed Encryption Standard).
 *      Algorithm developed by Xuejia Lai and James L. Massey, of ETH Zurich.
 *      This implementation modified and derived from original C code
 *      developed by Xuejia Lai.
 *      Zero-based indexing added, names changed from IPES to IDEA.
 *      CFB functions added.  Random number routines added.
 *
 *      Extensively optimized and restructured by Colin Plumb.
 *
 *      The IDEA(tm) block cipher is covered by patents held by ETH and a
 *      Swiss company called Ascom-Tech AG.  The Swiss patent number is
 *      PCT/CH91/00117, the European patent number is EP 0 482 154 B1, and
 *      the U.S. patent number is US005214703.  IDEA(tm) is a trademark of
 *      Ascom-Tech AG.  There is no license fee required for noncommercial
 *      use.  Commercial users may obtain licensing details from Dieter
 *      Profos, Ascom Tech AG, Solothurn Lab, Postfach 151, 4502 Solothurn,
 *      Switzerland, Tel +41 65 242885, Fax +41 65 235761.
 *
 *      The IDEA block cipher uses a 64-bit block size, and a 128-bit key
 *      size.  It breaks the 64-bit cipher block into four 16-bit words
 *      because all of the primitive inner operations are done with 16-bit
 *      arithmetic.  It likewise breaks the 128-bit cipher key into eight
 *      16-bit words.
 *
 *      For further information on the IDEA cipher, see the book:
 *        Xuejia Lai, "On the Design and Security of Block Ciphers",
 *        ETH Series on Information Processing (ed. J.L. Massey) Vol 1,
 *        Hartung-Gorre Verlag, Konstanz, Switzerland, 1992.  ISBN
 *        3-89191-573-X.
 *
 *      This code runs on arrays of bytes by taking pairs in big-endian
 *      order to make the 16-bit words that IDEA uses internally.  This
 *      produces the same result regardless of the byte order of the
 *      native CPU.
 */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <asm/byteorder.h>
#include <linux/crypto.h>
#include <linux/string.h>

#ifdef MODULE_LICENSE
MODULE_LICENSE("unknown");
#endif /* MODULE_LICENSE */
#ifdef MODULE_DESCRIPTION
MODULE_DESCRIPTION ("IDEA Cipher / CryptoAPI");
#endif /* MODULE_DESCRIPTION */

#define IDEAKEYSIZE 16
#define IDEABLOCKSIZE 8

#define IDEAROUNDS 8
#define IDEAKEYLEN (6*IDEAROUNDS+4)

#define low16(x) ((x) & 0xFFFF)

typedef struct {
	u16 ek[IDEAKEYLEN];
	u16 dk[IDEAKEYLEN];
} idea_key_t;

/* 
 * Compute the multiplicative inverse of x, modulo 65537, using
 * Euclid's algorithm. It is unrolled twice to avoid swapping the
 * registers each iteration, and some subtracts of t have been changed
 * to adds.
 */
static inline u16
mulInv (u16 x)
{
	u16 t0, t1;
	u16 q, y;

	if (x <= 1)
		return x;	/* 0 and 1 are self-inverse */
	t1 = 0x10001L / x;	/* Since x >= 2, this fits into 16 bits */
	y = 0x10001L % x;
	if (y == 1)
		return low16 (1 - t1);
	t0 = 1;
	do {
		q = x / y;
		x %= y;
		t0 += q * t1;
		if (x == 1)
			return t0;
		q = y / x;
		y %= x;
		t1 += q * t0;
	} while (y != 1);
	return low16 (1 - t1);
}

/*
 * Expand a 128-bit user key to a working encryption key ek
 */
static void
ideaExpandKey (const u8 *userkey, u16 *ek)
{
	int i, j;

	for (j = 0; j < 8; j++) {
		ek[j] = (userkey[0] << 8) + userkey[1];
		userkey += 2;
	}
	for (i = 0; j < IDEAKEYLEN; j++) {
		i++;
		ek[i + 7] = ek[i & 7] << 9 | ek[(i + 1) & 7] >> 7;
		ek += i & 8;
		i &= 7;
	}
}

/* 
 * Compute IDEA decryption key dk from an expanded IDEA encryption key
 * ek Note that the input and output may be the same.  Thus, the key
 * is inverted into an internal buffer, and then copied to the output.
 */
static void
ideaInvertKey (const u16 *ek, u16 dk[IDEAKEYLEN])
{
	int i;
	u16 t1, t2, t3;
	u16 temp[IDEAKEYLEN];
	u16 *p = temp + IDEAKEYLEN;

	t1 = mulInv (*ek++);
	t2 = -*ek++;
	t3 = -*ek++;
	*--p = mulInv (*ek++);
	*--p = t3;
	*--p = t2;
	*--p = t1;

	for (i = 0; i < IDEAROUNDS - 1; i++) {
		t1 = *ek++;
		*--p = *ek++;
		*--p = t1;

		t1 = mulInv (*ek++);
		t2 = -*ek++;
		t3 = -*ek++;
		*--p = mulInv (*ek++);
		*--p = t2;
		*--p = t3;
		*--p = t1;
	}
	t1 = *ek++;
	*--p = *ek++;
	*--p = t1;

	t1 = mulInv (*ek++);
	t2 = -*ek++;
	t3 = -*ek++;
	*--p = mulInv (*ek++);
	*--p = t3;
	*--p = t2;
	*--p = t1;
	/* Copy and destroy temp copy */
	memcpy (dk, temp, sizeof (temp));
	memset (&temp, 0, sizeof (temp));
}

/* IDEA encryption/decryption algorithm */
/* Note that in and out can be the same buffer */
static void
ideaCipher (const u16 inbuf[4], u16 outbuf[4], const u16 *key)
{
	register u16 x1, x2, x3, x4, s2, s3;
	register u16 t16;	/* Temporaries needed by MUL macro */
	register u32 t32;
	int r = IDEAROUNDS;

	x1 = be16_to_cpu (inbuf[0]);
	x2 = be16_to_cpu (inbuf[1]);
	x3 = be16_to_cpu (inbuf[2]);
	x4 = be16_to_cpu (inbuf[3]);

#define MUL(x,y) \
        ((t16 = (y)) ? \
                (x=low16(x)) ? \
                        t32 = (u32)x*t16, \
                        x = low16(t32), \
                        t16 = t32>>16, \
                        x = (x-t16)+(x<t16) \
                : \
                        (x = 1-t16) \
        : \
                (x = 1-x))

	do {
		MUL (x1, *key++);
		x2 += *key++;
		x3 += *key++;
		MUL (x4, *key++);

		s3 = x3;
		x3 ^= x1;
		MUL (x3, *key++);
		s2 = x2;
		x2 ^= x4;
		x2 += x3;
		MUL (x2, *key++);
		x3 += x2;

		x1 ^= x2;
		x4 ^= x3;

		x2 ^= s3;
		x3 ^= s2;
	} while (--r);

	MUL (x1, *key++);
	x3 += *key++;
	x2 += *key++;
	MUL (x4, *key);

#undef MUL

	outbuf[0] = cpu_to_be16 (x1);
	outbuf[1] = cpu_to_be16 (x3);
	outbuf[2] = cpu_to_be16 (x2);
	outbuf[3] = cpu_to_be16 (x4);
}

int
idea_encrypt (struct cipher_context *cx,
	      const u8 *in, u8 *out, int size, int atomic)
{
	const idea_key_t *c = (idea_key_t *) cx->keyinfo;

	if (size != 8)
		return 1;

	ideaCipher ((const u16 *) in, (u16 *) out, c->ek);

	return 0;
}

int
idea_decrypt (struct cipher_context *cx,
	      const u8 *in, u8 *out, int size, int atomic)
{
	const idea_key_t *c = (idea_key_t *) cx->keyinfo;

	if (size != 8)
		return 1;

	ideaCipher ((const u16 *) in, (u16 *) out, c->dk);

	return 0;
}

int
idea_set_key (struct cipher_context *cx,
	      const u8 *key, int key_len, int atomic)
{
	idea_key_t *c = (idea_key_t *) cx->keyinfo;

	if (key_len != 16)
		return -EINVAL;

	cx->key_length = key_len;

	ideaExpandKey (key, c->ek);
	ideaInvertKey (c->ek, c->dk);
	return 0;
}

#define CIPHER_ID                idea
#define CIPHER_BLOCKSIZE         64
#define CIPHER_KEY_SIZE_MASK     CIPHER_KEYSIZE_128
#define CIPHER_KEY_SCHEDULE_SIZE sizeof (idea_key_t)

#include "gen-cipher.h"

EXPORT_NO_SYMBOLS;

/* eof */
