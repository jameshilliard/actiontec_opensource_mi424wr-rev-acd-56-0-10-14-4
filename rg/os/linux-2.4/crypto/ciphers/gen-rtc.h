/* $Id: gen-rtc.h,v 1.1.1.1 2006/01/20 03:16:22 zjjiang Exp $
 *
 * jlcooke's impl'n of the RTC mode of operation
 * references: none known.  This was my idea to fix the "update" security
 * flaw with using CTR in loopback filesystems.
 * 
 * Comments to jlcooke@certainkey.com welcome / requested.
 * 
 * This code is licenced under the GNU General Public License.
 */

#include <linux/string.h>
#include <linux/sched.h>

#if defined(CIPHER_BITS_128)
# define BS 16
#else
# define BS 8
#endif

/*  
 * These functions only use the XOR operator on the data, so no
 * endianness problems should occur.
 */

#ifdef WANT_COMPUTEIV
static int
CIPHER_NAME(_rtc_seek_iv)(u8 *iv_out, const u8 *_iv, u32 offset) {
	const u32 *iv = (const u32 *) _iv;
	u8 ctr[BS];

	((u32 *)ctr)[0] = iv[0]; 
	((u32 *)ctr)[1] = iv[1];
#ifdef CIPHER_BITS_128
	((u32 *)ctr)[2] = iv[2]; 
	((u32 *)ctr)[3] = iv[3];
#endif

	/* increment our counter */
	if ((ctr[0] += offset) < offset) /* overflow */
		if (++ctr[1] == 0)
			if (++ctr[2] == 0)
				++ctr[3];

	memcpy(iv_out, ctr, sizeof(ctr));
	memset(ctr, 0, sizeof(ctr));

	return 0;
}
#endif /* WANT_COMPUTEIV */

/* RTC mode:
 * C = E_k(P xor iv)
 * P = iv xor D_k(C)
 */
static int
CIPHER_NAME(_rtc_encrypt)(struct cipher_context *cx, const u8 *in_blk, 
			  u8 *out_blk, int size, int atomic, const u8 *_iv)
{
	const u32 *iv = (const u32 *) _iv;
	u8 tmp[BS], ctr[BS];

	((u32 *)ctr)[0] = iv[0]; 
	((u32 *)ctr)[1] = iv[1];
#ifdef CIPHER_BITS_128
	((u32 *)ctr)[2] = iv[2]; 
	((u32 *)ctr)[3] = iv[3];
#endif
	
	for (size -= BS; size >= 0; size -= BS) {
		((u32 *)tmp)[0] = ctr[0] ^ ((u32 *)in_blk)[0];
		((u32 *)tmp)[1] = ctr[1] ^ ((u32 *)in_blk)[1];
#ifdef CIPHER_BITS_128
		((u32 *)tmp)[2] = ctr[2] ^ ((u32 *)in_blk)[2];
		((u32 *)tmp)[3] = ctr[3] ^ ((u32 *)in_blk)[3];
#endif
		/* increment our counter */
		if (++ctr[0] == 0)
			if (++ctr[1] == 0)
				if (++ctr[2] == 0)
					++ctr[3];

		CIPHER_NAME(_encrypt)(cx, tmp, tmp, BS, atomic);
		memcpy(out_blk, tmp, sizeof(tmp));
		in_blk += BS; out_blk += BS;

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,5,0)
		if (!atomic && current->need_resched)
			schedule ();
#else
		if (!atomic)
			cond_resched ();
#endif
	} /* for */

	if (size != -BS) {
		memset(tmp, 0, sizeof(tmp));
		memcpy(tmp, in_blk, size+BS);

		((u32 *)tmp)[0] ^= ctr[0];
		((u32 *)tmp)[1] ^= ctr[1];
#ifdef CIPHER_BITS_128
		((u32 *)tmp)[2] ^= ctr[2];
		((u32 *)tmp)[3] ^= ctr[3];
#endif
		CIPHER_NAME(_encrypt)(cx, tmp, out_blk, BS, atomic);
	} /* if */

	memset(tmp, 0, sizeof(tmp));
	memset(ctr, 0, sizeof(ctr));
	return 0;
}

/*
 * RTC mode:
 * C = E_k(P xor iv)
 * P = iv xor D_k(C)
 */
static int
CIPHER_NAME(_rtc_decrypt)(struct cipher_context *cx, const u8 *in_blk, 
			  u8 *out_blk, int size, int atomic, const u8 *_iv)
{
	const u32 *iv = (const u32 *) _iv;
	u8 tmp[BS], ctr[BS];
	
	((u32 *)ctr)[0] = iv[0]; 
	((u32 *)ctr)[1] = iv[1];
#ifdef CIPHER_BITS_128
	((u32 *)ctr)[2] = iv[2]; 
	((u32 *)ctr)[3] = iv[3];
#endif

	for (size -= BS; size >= 0; size -= BS) {
		CIPHER_NAME(_decrypt)(cx, in_blk, tmp, BS, atomic);
		((u32 *)tmp)[0] ^= ((u32 *)ctr)[0];
		((u32 *)tmp)[1] ^= ((u32 *)ctr)[1];
#ifdef CIPHER_BITS_128
		((u32 *)tmp)[2] ^= ((u32 *)ctr)[2];
		((u32 *)tmp)[3] ^= ((u32 *)ctr)[3];
#endif

		/* increment our counter */
		if (++ctr[0] == 0)
			if (++ctr[1] == 0)
				if (++ctr[2] == 0)
					++ctr[3];

		memcpy(out_blk, tmp, BS);
		in_blk += BS; out_blk += BS;

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,5,0)
		if (!atomic && current->need_resched)
			schedule ();
#else
		if (!atomic)
			cond_resched ();
#endif
	}

	if (size != -BS) { 
		CIPHER_NAME(_decrypt)(cx, in_blk, tmp, BS, atomic);
		((u32 *)tmp)[0] ^= ((u32 *)ctr)[0];
		((u32 *)tmp)[1] ^= ((u32 *)ctr)[1];
#ifdef CIPHER_BITS_128
		((u32 *)tmp)[2] ^= ((u32 *)ctr)[2];
		((u32 *)tmp)[3] ^= ((u32 *)ctr)[3];
#endif
		memcpy(out_blk, tmp, size+BS);
	}

	memset(tmp, 0, sizeof(tmp));
	memset(ctr, 0, sizeof(ctr));
	return 0;
}
#undef BS
