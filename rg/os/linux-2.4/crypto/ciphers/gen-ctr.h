/* $Id: gen-ctr.h,v 1.1.1.1 2006/01/20 03:16:22 zjjiang Exp $
 *
 * jlcooke's impl'n of the CTR mode of operation
 * references: http://csrc.nist.gov/publications/nistpubs/800-38a/sp800-38a.pdf
 *             Modes of operation with the Advanced Encryption Standard
 *
 * SECURITY WARNING!
 * -----------------
 * It should be noted that CTR mode of operation has the restriction that no
 * key-counter pair can be ever be used with different plaintexts.
 * 
 * If this should ever happen, comprimising of the key (and all plaintext)
 * will occur.
 * 
 * For this reason, CTR MUST NOT be used for encrypted loopback filesystems.
 * Use jlcooke's RTC mode instead, it is believed to be impervious to this
 * attack.
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
CIPHER_NAME(_ctr_seek_iv)(u8 *iv_out, const u8 *_iv, u32 offset) {
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

/* CTR mode:
 * C = P xor E_k(iv)
 * P = C xor E_k(iv)
 */
static int
CIPHER_NAME(_ctr_encrypt)(struct cipher_context *cx,
			  const u8 *in_blk, u8 *out_blk, int size,
			  int atomic, const u8 *_iv)
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
		CIPHER_NAME(_encrypt)(cx, ctr, tmp, BS, atomic);
		/* increment our counter */
		if (++ctr[0] == 0)
			if (++ctr[1] == 0)
				if (++ctr[2] == 0)
					++ctr[3];

		((u32 *)tmp)[0] ^= ((u32 *)in_blk)[0];
		((u32 *)tmp)[1] ^= ((u32 *)in_blk)[1];
#ifdef CIPHER_BITS_128
		((u32 *)tmp)[2] ^= ((u32 *)in_blk)[2];
		((u32 *)tmp)[3] ^= ((u32 *)in_blk)[3];
#endif
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
		/* it's ok to destroy ctr here, we will not need it again */
		CIPHER_NAME(_encrypt)(cx, ctr, ctr, BS, atomic);
		((u32 *)out_blk)[0 - BS/4] = ((u32 *)tmp)[0] ^ ((u32 *)ctr)[0];
		((u32 *)out_blk)[1 - BS/4] = ((u32 *)tmp)[1] ^ ((u32 *)ctr)[1];
#ifdef CIPHER_BITS_128
		((u32 *)out_blk)[2 - BS/4] = ((u32 *)tmp)[2] ^ ((u32 *)ctr)[2];
		((u32 *)out_blk)[3 - BS/4] = ((u32 *)tmp)[3] ^ ((u32 *)ctr)[3];
#endif
	} /* if */

	memset(tmp, 0, sizeof(tmp));
	memset(ctr, 0, sizeof(ctr));
	return 0;
}

/* CTR mode:
 * C = P xor E_k(iv)
 * P = C xor E_k(iv)
 */
static int
CIPHER_NAME(_ctr_decrypt)(struct cipher_context *cx,
			  const u8 *in_blk, u8 *out_blk, int size,
			  int atomic, const u8 *_iv)
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
		CIPHER_NAME(_encrypt)(cx, ctr, tmp, BS, atomic);
		/* increment our counter */
		if (++ctr[0] == 0)
			if (++ctr[1] == 0)
				if (++ctr[2] == 0)
					++ctr[3];

		((u32 *)tmp)[0] ^= ((u32 *)in_blk)[0];
		((u32 *)tmp)[1] ^= ((u32 *)in_blk)[1];
#ifdef CIPHER_BITS_128
		((u32 *)tmp)[2] ^= ((u32 *)in_blk)[2];
		((u32 *)tmp)[3] ^= ((u32 *)in_blk)[3];
#endif

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
		memset(tmp, 0, sizeof(tmp));
		memcpy(tmp, in_blk, size+BS);
		/* it's ok to destroy ctr here, we will not need it again */
		CIPHER_NAME(_encrypt)(cx, ctr, ctr, BS, atomic);
		((u32 *)out_blk)[0 - BS/4] = ((u32 *)tmp)[0] ^ ((u32 *)ctr)[0];
		((u32 *)out_blk)[1 - BS/4] = ((u32 *)tmp)[1] ^ ((u32 *)ctr)[1];
#ifdef CIPHER_BITS_128
		((u32 *)out_blk)[2 - BS/4] = ((u32 *)tmp)[2] ^ ((u32 *)ctr)[2];
		((u32 *)out_blk)[3 - BS/4] = ((u32 *)tmp)[3] ^ ((u32 *)ctr)[3];
#endif
	}

	memset(tmp, 0, sizeof(tmp));
	memset(ctr, 0, sizeof(ctr));
	return 0;
}
#undef BS
