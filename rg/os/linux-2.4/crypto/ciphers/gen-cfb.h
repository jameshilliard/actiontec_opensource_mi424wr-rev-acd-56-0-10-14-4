/*
  Cipher Feedback Block mode
 
  Written by David Bryson <mutex@mac.com>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software

   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

static int CIPHER_NAME(_cfb_encrypt)(struct cipher_context *cx, const u8 *in_blk,
				     u8 *out_blk, int size, int atomic, const u8 *_iv)
{
	const u32 *iv = (const u32 *) _iv;
	u8 tmp[BS];
	((u32 *)tmp)[0] = iv[0];
	((u32 *)tmp)[1] = iv[1];
#ifdef CIPHER_BITS_128
	((u32 *)tmp)[2] = iv[2];
	((u32 *)tmp)[3] = iv[3];
#endif
	for (size -= BS; size >= 0; size -= BS) {
		CIPHER_NAME(_encrypt)(cx, tmp, tmp, BS, atomic);
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
			schedule();
#else
		if (!atomic)
			cond_resched();
#endif	       
	} /* end for */

	if (size != BS) {
		memset(tmp, 0, sizeof(tmp));
		memcpy(tmp, in_blk, size+BS);
		CIPHER_NAME(_encrypt)(cx, tmp, tmp, BS, atomic);

		((u32 *)tmp)[0] ^= ((u32 *)in_blk)[0 - BS/4];
		((u32 *)tmp)[1] ^= ((u32 *)in_blk)[1 - BS/4];
#ifdef CIPHER_BITS_128
		((u32 *)tmp)[2] ^= ((u32 *)in_blk)[2 - BS/4];
		((u32 *)tmp)[3] ^= ((u32 *)in_blk)[3 - BS/4];
#endif
	} /* end if */

	return 0;
}

static int CIPHER_NAME(_cfb_decrypt)(struct cipher_context *cx, const u8 *in_blk,
				     u8 *out_blk, int size, int atomic, const u8 *_iv)
{
	const u32 *iv = (const u32 *) _iv;
	u8 tmp[BS];
	((u32 *)tmp)[0] = iv[0];
	((u32 *)tmp)[1] = iv[1];
#ifdef CIPHER_BITS_128
	((u32 *)tmp)[2] = iv[2];
	((u32 *)tmp)[3] = iv[3];
#endif
	for (size -= BS; size >= 0; size -= BS) {
		CIPHER_NAME(_encrypt)(cx, tmp, tmp, BS, atomic);
		((u32 *)tmp)[0] ^= ((u32 *)in_blk)[0];
		((u32 *)tmp)[1] ^= ((u32 *)in_blk)[1];
#ifdef CIPHER_BITS_128
		((u32 *)tmp)[2] ^= ((u32 *)in_blk)[2];
		((u32 *)tmp)[3] ^= ((u32 *)in_blk)[3];
#endif
		memcpy(out_blk, tmp, sizeof(tmp));
		memcpy(tmp, in_blk, sizeof(tmp));
		in_blk += BS; out_blk += BS;

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,5,0)
		if (!atomic && current->need_resched)
			schedule();
#else
		if (!atomic)
			cond_resched();
#endif	       
	} /* end for */

	if (size != -BS) { 
		CIPHER_NAME(_encrypt)(cx, tmp, tmp, BS, atomic);
		((u32 *)tmp)[0] ^= ((u32 *)in_blk)[0];
		((u32 *)tmp)[1] ^= ((u32 *)in_blk)[1];
#ifdef CIPHER_BITS_128
		((u32 *)tmp)[2] ^= ((u32 *)in_blk)[2];
		((u32 *)tmp)[3] ^= ((u32 *)in_blk)[3];
#endif
		memcpy(out_blk, tmp, size+BS);
	} /* end if */

	return 0;
}

#undef BS
