/* NOTE: This implementation has been changed from the original
   source.  See ChangeLog for more information.
   Maintained by Alexander Kjeldaas <astor@fast.no>
 */

/* This is an independent implementation of the DFC encryption      */
/* algorithm designed by a team at CNRS and France Telecom and      */
/* submitted as a candidate in the US NIST Advanced Encryption      */
/* Standard (AES) programme.                                        */
/*                                                                  */
/* Copyright in this implementation is held by Dr B R Gladman but   */
/* I hereby give permission for its free direct or derivative use   */
/* subject to acknowledgment of its origin and compliance with any  */
/* conditions that the originators of DFC place on its use.         */
/*                                                                  */
/* My thanks go to Serge Vaudenay of the Ecole Normale Superieure   */
/* for providing test vectors. This implementation has also been    */
/* tested with an independent implementation by Dr Russell Bradford */
/* (Department of Mathematical Sciences, University of Bath, Bath,  */
/* UK) and checks out.   My thanks go to Russell for his help in    */
/* comparing our implementations and finding bugs (and for help in  */
/* resolving 'endian' issues before test vectors became available). */
/*                                                                  */
/* Dr Brian Gladman (gladman@seven77.demon.co.uk) 27th July 1998    */
/*                                                                  */

/* The EES string is as follows (the abstract contains an error in 
   the last line of this sequence which changes KC and KD):

    0xb7e15162, 0x8aed2a6a, 0xbf715880, 0x9cf4f3c7, 
    0x62e7160f, 0x38b4da56, 0xa784d904, 0x5190cfef, 
    0x324e7738, 0x926cfbe5, 0xf4bf8d8d, 0x8c31d763,
    0xda06c80a, 0xbb1185eb, 0x4f7c7b57, 0x57f59584, 

    0x90cfd47d, 0x7c19bb42, 0x158d9554, 0xf7b46bce, 
    0xd55c4d79, 0xfd5f24d6, 0x613c31c3, 0x839a2ddf,
    0x8a9a276b, 0xcfbfa1c8, 0x77c56284, 0xdab79cd4, 
    0xc2b3293d, 0x20e9e5ea, 0xf02ac60a, 0xcc93ed87, 

    0x4422a52e, 0xcb238fee, 0xe5ab6add, 0x835fd1a0,
    0x753d0a8f, 0x78e537d2, 0xb95bb79d, 0x8dcaec64, 
    0x2c1e9f23, 0xb829b5c2, 0x780bf387, 0x37df8bb3, 
    0x00d01334, 0xa0d0bd86, 0x45cbfa73, 0xa6160ffe,

    0x393c48cb, 0xbbca060f, 0x0ff8ec6d, 0x31beb5cc, 
    0xeed7f2f0, 0xbb088017, 0x163bc60d, 0xf45a0ecb, 
    0x1bcd289b, 0x06cbbfea, 0x21ad08e1, 0x847f3f73,
    0x78d56ced, 0x94640d6e, 0xf0d3d37b, 0xe67008e1, 
    
    0x86d1bf27, 0x5b9b241d, 0xeb64749a, 0x47dfdfb9, 

    Where:

    EES = RT(0) | RT(1) | ... | RT(63) | KD | KC

    Note that the abstract describing DFC is written 
    in big endian notation with the most significant 
    digits of a sequence of digits placed at the low
    index positions in arrays. This format is used
    here and is only converted to machine format at
    the point that maths is done on any numbers in 
    the round function.
    
    The key input is thus treated as an array of 32 
    bit words numbered from 0..3, 0..5 or 0..7 
    depending on key length.  The first (leftmost) 
    bit of this key string as defined in the DFC 
    abstract is the most significant bit of word 0 
    and the rightmost bit of this string is the least 
    signicant bit of the highest numbered key word.

    The input and output blocks for the cipher are 
    also treated as arrays of 32 bit words numbered
    from 0..3.  The most significant bit of word 0 is
    the 1st (leftmost) bit of the 128 bit input string 
    and the least significant bit of word 3 is the 
    last (rightmost) bit.
    
    Note that the inputs, the output and the key are
    in Intel little endian format when BYTE_SWAP is 
    defined

Timing data:

Algorithm: dfc (dfc2.c)
128 bit key:
Key Setup:    7373 cycles
Encrypt:      1748 cycles =    14.6 mbits/sec
Decrypt:      1755 cycles =    14.6 mbits/sec
Mean:         1752 cycles =    14.6 mbits/sec
192 bit key:
Key Setup:    7359 cycles
Encrypt:      1757 cycles =    14.6 mbits/sec
Decrypt:      1765 cycles =    14.5 mbits/sec
Mean:         1761 cycles =    14.5 mbits/sec
256 bit key:
Key Setup:    7320 cycles
Encrypt:      1750 cycles =    14.6 mbits/sec
Decrypt:      1749 cycles =    14.6 mbits/sec
Mean:         1749 cycles =    14.6 mbits/sec

*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/crypto.h>
#include <asm/byteorder.h>

#ifdef MODULE_LICENSE
MODULE_LICENSE("Dual BSD/GPL");
#endif
#ifdef MODULE_DESCRIPTION
MODULE_DESCRIPTION ("DFC Cipher / CryptoAPI");
#endif
#ifdef MODULE_AUTHOR
MODULE_AUTHOR ("Dr Brian Gladman <gladman@seven77.demon.co.uk>");
#endif

#define io_swap(x)  __cpu_to_be32(x)

/* The following arrays are all stored in big endian    */
/* format with 32 bit words at lower array positions    */
/* being more significant in multi-word values          */

static const u32 rt64[64] = 
{
    0xb7e15162, 0x8aed2a6a, 0xbf715880, 0x9cf4f3c7, 
    0x62e7160f, 0x38b4da56, 0xa784d904, 0x5190cfef, 
    0x324e7738, 0x926cfbe5, 0xf4bf8d8d, 0x8c31d763,
    0xda06c80a, 0xbb1185eb, 0x4f7c7b57, 0x57f59584, 

    0x90cfd47d, 0x7c19bb42, 0x158d9554, 0xf7b46bce, 
    0xd55c4d79, 0xfd5f24d6, 0x613c31c3, 0x839a2ddf,
    0x8a9a276b, 0xcfbfa1c8, 0x77c56284, 0xdab79cd4, 
    0xc2b3293d, 0x20e9e5ea, 0xf02ac60a, 0xcc93ed87, 

    0x4422a52e, 0xcb238fee, 0xe5ab6add, 0x835fd1a0,
    0x753d0a8f, 0x78e537d2, 0xb95bb79d, 0x8dcaec64, 
    0x2c1e9f23, 0xb829b5c2, 0x780bf387, 0x37df8bb3, 
    0x00d01334, 0xa0d0bd86, 0x45cbfa73, 0xa6160ffe,

    0x393c48cb, 0xbbca060f, 0x0ff8ec6d, 0x31beb5cc, 
    0xeed7f2f0, 0xbb088017, 0x163bc60d, 0xf45a0ecb, 
    0x1bcd289b, 0x06cbbfea, 0x21ad08e1, 0x847f3f73,
    0x78d56ced, 0x94640d6e, 0xf0d3d37b, 0xe67008e1, 
};

static const u32  kc = 0xeb64749a;

static const u32  kd2[2] = 
{
    0x86d1bf27, 0x5b9b241d  
};

static const u32  ka2[6] = 
{
    0xb7e15162, 0x8aed2a6a, 
    0xbf715880, 0x9cf4f3c7, 
    0x62e7160f, 0x38b4da56, 
};

static const u32  kb2[6] =
{
    0xa784d904, 0x5190cfef, 
    0x324e7738, 0x926cfbe5, 
    0xf4bf8d8d, 0x8c31d763,
};

static const u32  ks8[8] = 
{   0xda06c80a, 0xbb1185eb, 0x4f7c7b57, 0x57f59584, 
    0x90cfd47d, 0x7c19bb42, 0x158d9554, 0xf7b46bce,
};

#define lo(x)   ((x) & 0x0000ffff)
#define hi(x)   ((x) >> 16)

static void mult_64(u32 r[4], const u32 x[2], const u32 y[2])
{   u32  x0, x1, x2, x3, y0, y1, y2, y3, t0, t1, t2, t3, c;

    x0 = lo(x[1]); x1 = hi(x[1]); x2 = lo(x[0]); x3 = hi(x[0]);
    y0 = lo(y[1]); y1 = hi(y[1]); y2 = lo(y[0]); y3 = hi(y[0]);

    t0 = x0 * y0; r[0] = lo(t0); c = hi(t0);

    t0 = x0 * y1; t1 = x1 * y0; c += lo(t0) + lo(t1);
    r[0] += (c << 16); c = hi(c) + hi(t0) + hi(t1);

    t0 = x0 * y2; t1 = x1 * y1; t2 = x2 * y0;
    c += lo(t0) + lo(t1) + lo(t2); r[1] = lo(c);
    c = hi(c) + hi(t0) + hi(t1) + hi(t2);

    t0 = x0 * y3; t1 = x1 * y2; t2 = x2 * y1; t3 = x3 * y0;
    c += lo(t0) + lo(t1) + lo(t2) + lo(t3); r[1] += (c << 16); 
    c = hi(c) + hi(t0) + hi(t1) + hi(t2) + hi(t3);

    t0 = x1 * y3; t1 = x2 * y2; t2 = x3 * y1;
    c += lo(t0) + lo(t1) + lo(t2); r[2] = lo(c);
    c = hi(c) + hi(t0) + hi(t1) + hi(t2);

    t0 = x2 * y3; t1 = x3 * y2; c += lo(t0) + lo(t1);
    r[2] += (c << 16); c = hi(c) + hi(t0) + hi(t1);

    r[3] = c + x3 * y3;
};

inline static void add_64(u32 r[4], const u32 hi, const u32 lo)
{
    if((r[0] += lo) < lo)
        if(!++r[1])
            if(!++r[2])
                ++r[3];

    if((r[1] += hi) < hi)
        if(!++r[2])
            ++r[3];
};

static void mult_13(u32 r[3])
{   u32  c, d;

    c = 13 * lo(r[0]);
    d = hi(r[0]);
    r[0] = lo(c);
    c = hi(c) + 13 * d;
    r[0] += (c << 16);
    c = hi(c) + 13 * lo(r[1]);
    d = hi(r[1]);
    r[1] = lo(c); 
    c = hi(c) + 13 * d;
    r[1] += (c << 16);
    r[2] = hi(c);
};

/* Where necessary this is where conversion from big endian to  */
/* little endian format is performed.  Since all the maths is   */
/* little endian care is needed when 64 bit blocks are being    */
/* used to get them in the right order by reversing the order   */
/* in which these are stored. This applies to the key array     */
/* which gives the two values A and B and to the constant KD.   */
/* Since the input and output blocks are big endian we also     */
/* have to invert the order of the 32 bit words in the 64 bit   */
/* blocks being processed.                                      */ 

static void r_fun(u32 outp[2], const u32 inp[2], const u32 key[4])
{   u32 acc[5], b, t;

    mult_64(acc, inp, key);  add_64(acc, key[2], key[3]);

    /* we need the value in the accumulator mod 2^64 + 13 so if */
    /* the accumulator value is hi * 2^64 + lo we need to find  */
    /* a k value such that r = hi * 2^64 + lo - k * (2^64 + 13) */
    /* is 0 <= r < 2^64 + 13.  We can see that k will be close  */
    /* to hi in value - it may equal hi but will not be greater */
    /* and we can let k = hi - e with e >= 0 so that r is given */
    /* by r = e * (2^64 + 13) + lo - 13 * hi. If we compute the */
    /* lo - 13 * hi value, the overflow into the top 64 bits of */
    /* the accumulator has to be 'zeroed' by the e * (2^64 + 13)*/
    /* term and this sets the e value (in fact such an overlow  */
    /* is only removed when the lower word is higher than 12).  */

    mult_13(&acc[2]);   /* multiply top of accumulator by 13    */

    /* calculate lo - 13 * hi in acc[0] and acc[1] with any     */
    /* overflow into top 64 bits in b                           */

    t = acc[0]; acc[0] -= acc[2]; b = (acc[0] > t ? 1 : 0);

    t = acc[1]; acc[1] -= acc[3] + b;
    b = (acc[1] > t ? 1 : (acc[1] == t ? b : 0));

    b = 13 * (acc[4] + b);  /* overflow into top 64 bits of acc */

    if(((acc[0] += b) < b) && !(++acc[1]))
    {
        if(acc[0] > 12)

            acc[0] -= 13;
    }

    /* do the confusion permutation */

    t = acc[1] ^ kc; b = acc[0] ^ rt64[acc[1] >> 26];  
    
    b += kd2[0] + ((t += kd2[1]) < kd2[1] ? 1 : 0);

    outp[0] ^= b; outp[1] ^= t; 
};

static int dfc_set_key(struct cipher_context *cx, const unsigned char *key, 
		       int key_len, int atomic)
{   u32 *in_key = (u32 *)key;
    /* l_key - storage for the key schedule */
    u32 *l_key   = cx->keyinfo;
    u32  i, lk[32], rk[4];

    if (key_len != 16 && key_len != 24 && key_len != 32)
      return -EINVAL; /* unsupported key length */

    cx->key_length = key_len;

    key_len *= 8;

    for(i = 0; i < key_len / 32; ++i)
      lk[i] = io_swap(in_key[i]);

    /* pad the key with the KS array            */

    for(i = 0; i < 8 - key_len / 32; ++i)    /* K|KS */
      lk[i + key_len / 32] = ks8[i];

    /* do the reordering of the key parameters  */
    /* the OAP[1]|OBP[1]|OAP[2]... sequence is  */
    /* at lk[0]... and the other at lk[16]...   */
    
    lk[18] = lk[5]; lk[19] = lk[2]; /* EBP */ 
    lk[16] = lk[1]; lk[17] = lk[6]; /* EAP */
    lk[ 2] = lk[4]; lk[ 3] = lk[3]; /* OBP */
    lk[ 0] = lk[0]; lk[ 1] = lk[7]; /* OAP */

    /* create other elements using KA and KB    */

    for(i = 0; i < 6; i += 2)
    {
        lk[i + i +   4] = lk[ 0] ^ ka2[i];      /* OAP[i] ms */
        lk[i + i +   5] = lk[ 1] ^ ka2[i + 1];  /* OAP[i] ls */
        lk[i + i +   6] = lk[ 2] ^ kb2[i];      /* OBP[i] ms */
        lk[i + i +   7] = lk[ 3] ^ kb2[i + 1];  /* OBP[i] ls */
        lk[i + i +  20] = lk[16] ^ ka2[i];      /* EAP[i] ms */
        lk[i + i +  21] = lk[17] ^ ka2[i + 1];  /* EAP[i] ls */
        lk[i + i +  22] = lk[18] ^ kb2[i];      /* EBP[i] ms */
        lk[i + i +  23] = lk[19] ^ kb2[i + 1];  /* EBP[i] ls */
    }

    rk[0] = rk[1] = rk[2] = rk[3] = 0;

    /* do the 4 round key mixing encryption     */

    for(i = 0; i < 32; i += 8)
    {
        r_fun(rk, rk + 2, lk);      /*  R2|R1   */
        r_fun(rk + 2, rk, lk +  4); /*  R2|R3   */
        r_fun(rk, rk + 2, lk +  8); /*  R4|R3   */
        r_fun(rk + 2, rk, lk + 12); /*  R4|R5   */

        /* keep key in big endian format with   */
        /* the most significant 32 bit words    */
        /* first (lowest) in the key schedule   */
        /* - note that the upper and lower 64   */
        /* bit blocks are in inverse order at   */
        /* this point in the loop               */

        l_key[i + 0] = rk[2]; l_key[i + 1] = rk[3]; 
        l_key[i + 2] = rk[0]; l_key[i + 3] = rk[1]; 

        r_fun(rk + 2, rk, lk + 16); /*  R1|R2   */
        r_fun(rk, rk + 2, lk + 20); /*  R3|R2   */
        r_fun(rk + 2, rk, lk + 24); /*  R3|R4   */  
        r_fun(rk, rk + 2, lk + 28); /*  R5|R4   */

        l_key[i + 4] = rk[0]; l_key[i + 5] = rk[1]; 
        l_key[i + 6] = rk[2]; l_key[i + 7] = rk[3];
    }
    
    return 0;
};

static int dfc_encrypt(struct cipher_context *cx, const u8 *in, u8 *out, 
		int size, int atomic)
{   u32 *l_key = cx->keyinfo;
    u32 *in_blk = (u32 *)in;
    u32 *out_blk = (u32 *)out;
    u32  blk[4];

    /* the input/output format is big endian -  */
    /* any reversals needed are performed when  */
    /* maths is done in the round function      */

    blk[0] = io_swap(in_blk[0]); blk[1] = io_swap(in_blk[1]);
    blk[2] = io_swap(in_blk[2]); blk[3] = io_swap(in_blk[3]);

    r_fun(blk, blk + 2, l_key +  0); /*  R2|R1  */ 
    r_fun(blk + 2, blk, l_key +  4); /*  R2|R3  */
    r_fun(blk, blk + 2, l_key +  8); /*  R4|R3  */
    r_fun(blk + 2, blk, l_key + 12); /*  R4|R5  */
    r_fun(blk, blk + 2, l_key + 16); /*  R6|R5  */
    r_fun(blk + 2, blk, l_key + 20); /*  R6|R7  */
    r_fun(blk, blk + 2, l_key + 24); /*  R8|R7  */
    r_fun(blk + 2, blk, l_key + 28); /*  R8|R9  */

    /* swap order to obtain the result R9|R8    */

    out_blk[0] = io_swap(blk[2]); out_blk[1] = io_swap(blk[3]);
    out_blk[2] = io_swap(blk[0]); out_blk[3] = io_swap(blk[1]);
    return 0;
};

static int dfc_decrypt(struct cipher_context *cx, const u8 *in, u8 *out, 
		int size, int atomic)
{   u32 *l_key = cx->keyinfo;
    u32 *in_blk = (u32 *)in;
    u32 *out_blk = (u32 *)out;
    u32 blk[4];

    /* the input/output format is big endian -  */
    /* any reversals needed are performed when  */
    /* maths is done in the round function      */

    blk[0] = io_swap(in_blk[0]); blk[1] = io_swap(in_blk[1]);
    blk[2] = io_swap(in_blk[2]); blk[3] = io_swap(in_blk[3]);

    r_fun(blk, blk + 2, l_key + 28); /*  R7|R8  */
    r_fun(blk + 2, blk, l_key + 24); /*  R7|R6  */
    r_fun(blk, blk + 2, l_key + 20); /*  R5|R6  */
    r_fun(blk + 2, blk, l_key + 16); /*  R5|R4  */
    r_fun(blk, blk + 2, l_key + 12); /*  R3|R4  */
    r_fun(blk + 2, blk, l_key +  8); /*  R3|R2  */
    r_fun(blk, blk + 2, l_key +  4); /*  R1|R2  */
    r_fun(blk + 2, blk, l_key     ); /*  R1|R0  */ 

    /* swap order to obtain the result R1|R0    */

    out_blk[0] = io_swap(blk[2]); out_blk[1] = io_swap(blk[3]);
    out_blk[2] = io_swap(blk[0]); out_blk[3] = io_swap(blk[1]);   
    return 0;
};

#define CIPHER_ID                dfc
#define CIPHER_BLOCKSIZE         128
#define CIPHER_KEY_SIZE_MASK     CIPHER_KEYSIZE_128 | CIPHER_KEYSIZE_192 | CIPHER_KEYSIZE_256
#define CIPHER_KEY_SCHEDULE_SIZE (32 * sizeof(u32))

#include "gen-cipher.h"

EXPORT_NO_SYMBOLS;

/* eof */
