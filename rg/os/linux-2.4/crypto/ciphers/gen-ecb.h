
#include <linux/string.h>
#include <linux/sched.h>

#if defined(CIPHER_BITS_128)
# define BS 16
#else
# define BS 8
#endif

static int CIPHER_NAME(_ecb_encrypt)(struct cipher_context *cx, const u8 *in_blk, 
				     u8 *out_blk, int size, int atomic, const u8 *_iv)
{
	u8 tmp[BS];
	for (size -= BS; size >= 0; size -= BS) {
		CIPHER_NAME(_encrypt)(cx, in_blk, out_blk, BS, atomic);
		in_blk += BS; out_blk += BS;
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,5,0)		
		if (!atomic && current->need_resched) {
			schedule();
		}
#else
		if (!atomic) {
			cond_resched();
		}
#endif		
	}
	if (size != -BS) {
		memset(tmp, 0, sizeof(tmp));
		memcpy(tmp, in_blk, size+BS);
		CIPHER_NAME(_encrypt)(cx, tmp, out_blk, BS, atomic);
	}
	return 0;
}

static int CIPHER_NAME(_ecb_decrypt)(struct cipher_context *cx, const u8 *in_blk, 
				     u8 *out_blk, int size, int atomic, const u8 *_iv)
{
	u8 tmp[BS];
	for (size -= BS; size >= 0; size -= BS) {
		CIPHER_NAME(_decrypt)(cx, in_blk, out_blk, BS, atomic);
		in_blk += BS; out_blk += BS;
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,5,0)		
		if (!atomic && current->need_resched) {
			schedule();
		}
#else
		if (!atomic) {
			cond_resched();
		}
#endif
	}
	if (size != -BS) {
		CIPHER_NAME(_decrypt)(cx, in_blk, tmp, BS, atomic);
		memcpy(out_blk, tmp, size+BS);
	}
	return 0;
}


#undef BS
