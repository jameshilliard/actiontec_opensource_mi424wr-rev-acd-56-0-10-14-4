/* $Id: digest-sha512.c,v 1.1.1.1 2006/01/20 03:16:22 zjjiang Exp $
 * 
 * SHA-512 code by Jean-Luc Cooke <jlcooke@certainkey.com>.
 * 
 * Glue code originally by Andrew McDonald and Alan Smithee, mailed
 * to maintainer on pulped trees.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/init.h>

#include <asm/uaccess.h>

#include <linux/crypto.h>
#include <linux/wordops.h>

#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif /* MODULE LICENSE */
#ifdef MODULE_DESCRIPTION
MODULE_DESCRIPTION ("SHA512 Digest / CryptoAPI");
#endif

#define SHA512_DIGEST_SIZE (8*sizeof(u64))

/* Disabling this will reduce the size of the code by a considerable
   amount, that you might be interested in reclaiming if you require
   smaller code. However, this will come at the cost of some speed. */
/* #define CONFIG_DIGEST_FAST */

struct SHA512_CTX {
	u64 state[8];
	u32 count[4];
	u8 buf[128];
};

#define Ch(x,y,z)   ((x & y) ^ (~x & z))
#define Maj(x,y,z)  ((x & y) ^ ( x & z) ^ (y & z))
#define RORu64(x,y) generic_rotr64(x, y)

#define e0(x)       (RORu64(x,28) ^ RORu64(x,34) ^ RORu64(x,39))
#define e1(x)       (RORu64(x,14) ^ RORu64(x,18) ^ RORu64(x,41))
#define s0(x)       (RORu64(x, 1) ^ RORu64(x, 8) ^ (x >> 7))
#define s1(x)       (RORu64(x,19) ^ RORu64(x,61) ^ (x >> 6))

#define H0         0x6a09e667f3bcc908
#define H1         0xbb67ae8584caa73b
#define H2         0x3c6ef372fe94f82b
#define H3         0xa54ff53a5f1d36f1
#define H4         0x510e527fade682d1
#define H5         0x9b05688c2b3e6c1f
#define H6         0x1f83d9abfb41bd6b
#define H7         0x5be0cd19137e2179

#if !defined(CONFIG_DIGEST_FAST)
const u64 sha512_K[80] = {
	0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f,
	0xe9b5dba58189dbbc, 0x3956c25bf348b538, 0x59f111f1b605d019,
	0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242,
	0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
	0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235,
	0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3,
	0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, 0x2de92c6f592b0275,
	0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
	0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f,
	0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725,
	0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc,
	0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
	0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6,
	0x92722c851482353b, 0xa2bfe8a14cf10364, 0xa81a664bbc423001,
	0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218,
	0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
	0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99,
	0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb,
	0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc,
	0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
	0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915,
	0xc67178f2e372532b, 0xca273eceea26619c, 0xd186b8c721c0c207,
	0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba,
	0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
	0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc,
	0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a,
	0x5fcb6fab3ad6faec, 0x6c44198c4a475817,
};
#endif /* !CONFIG_DIGEST_FAST */

#define LOAD_OP(I)\
 {\
  t1  = input[(8*I)  ] & 0xff;   t1<<=8;\
  t1 |= input[(8*I)+1] & 0xff;   t1<<=8;\
  t1 |= input[(8*I)+2] & 0xff;   t1<<=8;\
  t1 |= input[(8*I)+3] & 0xff;   t1<<=8;\
  t1 |= input[(8*I)+4] & 0xff;   t1<<=8;\
  t1 |= input[(8*I)+5] & 0xff;   t1<<=8;\
  t1 |= input[(8*I)+6] & 0xff;   t1<<=8;\
  t1 |= input[(8*I)+7] & 0xff;\
  W[I] = t1;\
 }
#define BLEND_OP(I) {\
  W[I  ] = s1(W[I-2]) + W[I-7] + s0(W[I-15]) + W[I-16];\
}

static void
SHA512Transform(u64 *state, const u8 *input)
{
	u64 a, b, c, d, e, f, g, h, t1, t2;
	u64 W[80];

#if !defined(CONFIG_DIGEST_FAST)
	int i;

	/* load the input */
	LOAD_OP( 0); LOAD_OP( 1); LOAD_OP( 2); LOAD_OP( 3);
	LOAD_OP( 4); LOAD_OP( 5); LOAD_OP( 6); LOAD_OP( 7);
	LOAD_OP( 8); LOAD_OP( 9); LOAD_OP(10); LOAD_OP(11);
	LOAD_OP(12); LOAD_OP(13); LOAD_OP(14); LOAD_OP(15);

	/* now blend */
	for (i=16; i<80; i+=8) {
		BLEND_OP(i  ); BLEND_OP(i+1); BLEND_OP(i+2); BLEND_OP(i+3);
		BLEND_OP(i+4); BLEND_OP(i+5); BLEND_OP(i+6); BLEND_OP(i+7);
	}

	/* load the state into our registers */
	a=state[0];   b=state[1];   c=state[2];   d=state[3];  
	e=state[4];   f=state[5];   g=state[6];   h=state[7];  
  
	/* now iterate */
	for (i=0; i<80; i+=8) {
		t1 = h + e1(e) + Ch(e,f,g) + sha512_K[i  ] + W[i  ];
		t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
		t1 = g + e1(d) + Ch(d,e,f) + sha512_K[i+1] + W[i+1];
		t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
		t1 = f + e1(c) + Ch(c,d,e) + sha512_K[i+2] + W[i+2];
		t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
		t1 = e + e1(b) + Ch(b,c,d) + sha512_K[i+3] + W[i+3];
		t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
		t1 = d + e1(a) + Ch(a,b,c) + sha512_K[i+4] + W[i+4];
		t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
		t1 = c + e1(h) + Ch(h,a,b) + sha512_K[i+5] + W[i+5];
		t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
		t1 = b + e1(g) + Ch(g,h,a) + sha512_K[i+6] + W[i+6];
		t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
		t1 = a + e1(f) + Ch(f,g,h) + sha512_K[i+7] + W[i+7];
		t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;
	}
#else /* CONFIG_DIGEST_FAST */
	/* load the input */
	LOAD_OP( 0); LOAD_OP( 1); LOAD_OP( 2); LOAD_OP( 3);
	LOAD_OP( 4); LOAD_OP( 5); LOAD_OP( 6); LOAD_OP( 7);
	LOAD_OP( 8); LOAD_OP( 9); LOAD_OP(10); LOAD_OP(11);
	LOAD_OP(12); LOAD_OP(13); LOAD_OP(14); LOAD_OP(15);

	/* now blend */
	BLEND_OP(16); BLEND_OP(17); BLEND_OP(18); BLEND_OP(19);
	BLEND_OP(20); BLEND_OP(21); BLEND_OP(22); BLEND_OP(23);
	BLEND_OP(24); BLEND_OP(25); BLEND_OP(26); BLEND_OP(27);
	BLEND_OP(28); BLEND_OP(29); BLEND_OP(30); BLEND_OP(31);
	BLEND_OP(32); BLEND_OP(33); BLEND_OP(34); BLEND_OP(35);
	BLEND_OP(36); BLEND_OP(37); BLEND_OP(38); BLEND_OP(39);
	BLEND_OP(40); BLEND_OP(41); BLEND_OP(42); BLEND_OP(43);
	BLEND_OP(44); BLEND_OP(45); BLEND_OP(46); BLEND_OP(47);
	BLEND_OP(48); BLEND_OP(49); BLEND_OP(50); BLEND_OP(51);
	BLEND_OP(52); BLEND_OP(53); BLEND_OP(54); BLEND_OP(55);
	BLEND_OP(56); BLEND_OP(57); BLEND_OP(58); BLEND_OP(59);
	BLEND_OP(60); BLEND_OP(61); BLEND_OP(62); BLEND_OP(63);
	BLEND_OP(64); BLEND_OP(65); BLEND_OP(66); BLEND_OP(67);
	BLEND_OP(68); BLEND_OP(69); BLEND_OP(70); BLEND_OP(71);
	BLEND_OP(72); BLEND_OP(73); BLEND_OP(74); BLEND_OP(75);
	BLEND_OP(76); BLEND_OP(77); BLEND_OP(78); BLEND_OP(79);
    
	/* load the state into our registers */
	a=state[0];   b=state[1];   c=state[2];   d=state[3];  
	e=state[4];   f=state[5];   g=state[6];   h=state[7];  
  
	/* now iterate */
	t1 = h + e1(e) + Ch(e,f,g) + 0x428a2f98d728ae22 + W[ 0];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0x7137449123ef65cd + W[ 1];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0xb5c0fbcfec4d3b2f + W[ 2];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0xe9b5dba58189dbbc + W[ 3];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0x3956c25bf348b538 + W[ 4];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0x59f111f1b605d019 + W[ 5];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0x923f82a4af194f9b + W[ 6];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0xab1c5ed5da6d8118 + W[ 7];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0xd807aa98a3030242 + W[ 8];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0x12835b0145706fbe + W[ 9];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0x243185be4ee4b28c + W[10];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0x550c7dc3d5ffb4e2 + W[11];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0x72be5d74f27b896f + W[12];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0x80deb1fe3b1696b1 + W[13];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0x9bdc06a725c71235 + W[14];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0xc19bf174cf692694 + W[15];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0xe49b69c19ef14ad2 + W[16];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0xefbe4786384f25e3 + W[17];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0x0fc19dc68b8cd5b5 + W[18];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0x240ca1cc77ac9c65 + W[19];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0x2de92c6f592b0275 + W[20];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0x4a7484aa6ea6e483 + W[21];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0x5cb0a9dcbd41fbd4 + W[22];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0x76f988da831153b5 + W[23];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0x983e5152ee66dfab + W[24];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0xa831c66d2db43210 + W[25];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0xb00327c898fb213f + W[26];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0xbf597fc7beef0ee4 + W[27];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0xc6e00bf33da88fc2 + W[28];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0xd5a79147930aa725 + W[29];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0x06ca6351e003826f + W[30];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0x142929670a0e6e70 + W[31];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0x27b70a8546d22ffc + W[32];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0x2e1b21385c26c926 + W[33];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0x4d2c6dfc5ac42aed + W[34];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0x53380d139d95b3df + W[35];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0x650a73548baf63de + W[36];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0x766a0abb3c77b2a8 + W[37];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0x81c2c92e47edaee6 + W[38];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0x92722c851482353b + W[39];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0xa2bfe8a14cf10364 + W[40];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0xa81a664bbc423001 + W[41];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0xc24b8b70d0f89791 + W[42];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0xc76c51a30654be30 + W[43];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0xd192e819d6ef5218 + W[44];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0xd69906245565a910 + W[45];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0xf40e35855771202a + W[46];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0x106aa07032bbd1b8 + W[47];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0x19a4c116b8d2d0c8 + W[48];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0x1e376c085141ab53 + W[49];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0x2748774cdf8eeb99 + W[50];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0x34b0bcb5e19b48a8 + W[51];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0x391c0cb3c5c95a63 + W[52];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0x4ed8aa4ae3418acb + W[53];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0x5b9cca4f7763e373 + W[54];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0x682e6ff3d6b2b8a3 + W[55];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0x748f82ee5defb2fc + W[56];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0x78a5636f43172f60 + W[57];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0x84c87814a1f0ab72 + W[58];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0x8cc702081a6439ec + W[59];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0x90befffa23631e28 + W[60];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0xa4506cebde82bde9 + W[61];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0xbef9a3f7b2c67915 + W[62];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0xc67178f2e372532b + W[63];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0xca273eceea26619c + W[64];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0xd186b8c721c0c207 + W[65];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0xeada7dd6cde0eb1e + W[66];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0xf57d4f7fee6ed178 + W[67];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0x06f067aa72176fba + W[68];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0x0a637dc5a2c898a6 + W[69];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0x113f9804bef90dae + W[70];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0x1b710b35131c471b + W[71];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0x28db77f523047d84 + W[72];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0x32caab7b40c72493 + W[73];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0x3c9ebe0a15c9bebc + W[74];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0x431d67c49c100d4c + W[75];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0x4cc5d4becb3e42b6 + W[76];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0x597f299cfc657e2a + W[77];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0x5fcb6fab3ad6faec + W[78];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0x6c44198c4a475817 + W[79];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;
#endif /* CONFIG_DIGEST_FAST */
  
	state[0] += a; state[1] += b; state[2] += c; state[3] += d;  
	state[4] += e; state[5] += f; state[6] += g; state[7] += h;  

	/* erase our data */
	a = b = c = d = e = f = g = h = t1 = t2 = 0;
	memset(W, 0, 80 * sizeof(u64));
}

static void
SHA512Init(struct SHA512_CTX *C)
{
	C->state[0] = H0;
	C->state[1] = H1;
	C->state[2] = H2;
	C->state[3] = H3;
	C->state[4] = H4;
	C->state[5] = H5;
	C->state[6] = H6;
	C->state[7] = H7;
	C->count[0] = C->count[1] = C->count[2] = C->count[3] = 0;
	memset(C->buf, 0, 128);
}

static void
SHA512Update(struct SHA512_CTX *C, const u8 *input, u32 inputLen)
{
	u32 i, index, partLen;

	/* Compute number of bytes mod 128 */
	index = (u32)((C->count[0] >> 3) & 0x7F);
	
	/* Update number of bits */
	if ((C->count[0] += (inputLen << 3)) < (inputLen << 3)) {
		if ((C->count[1] += 1) < 1)
			if ((C->count[2] += 1) < 1)
				C->count[3]++;
		C->count[1] += (inputLen >> 29);
	}
	
	partLen = 128 - index;
	
	/* Transform as many times as possible. */
	if (inputLen >= partLen) {
		memcpy(&C->buf[index], input, partLen);
		SHA512Transform(C->state, C->buf);

		for (i=partLen; i+127<inputLen; i+=128)
			SHA512Transform(C->state, &input[i]);

		index = 0;
	} else {
		i = 0;
	}

	/* Buffer remaining input */
	memcpy(&C->buf[index], &input[i], inputLen-i);
}

static void
SHA512Final(struct SHA512_CTX *C, u8 *digest)
{
	const static u8 padding[128] = { 0x80, };
	u8 bits[128] = { 0x00, };
	u32 t, index, padLen;
	u64 t2;
	int i,j;

	/* Save number of bits */
	t = C->count[0];
	bits[15] = t; t>>=8;
	bits[14] = t; t>>=8;
	bits[13] = t; t>>=8;
	bits[12] = t; 
	t = C->count[1];
	bits[11] = t; t>>=8;
	bits[10] = t; t>>=8;
	bits[9 ] = t; t>>=8;
	bits[8 ] = t; 
	t = C->count[2];
	bits[7 ] = t; t>>=8;
	bits[6 ] = t; t>>=8;
	bits[5 ] = t; t>>=8;
	bits[4 ] = t; 
	t = C->count[3];
	bits[3 ] = t; t>>=8;
	bits[2 ] = t; t>>=8;
	bits[1 ] = t; t>>=8;
	bits[0 ] = t; 

	/* Pad out to 112 mod 128. */
	index = (C->count[0] >> 3) & 0x7f;
	padLen = (index < 112) ? (112 - index) : ((128+112) - index);
	SHA512Update(C, padding, padLen);

	/* Append length (before padding) */
	SHA512Update(C, bits, 16);

	/* Store state in digest */
	for (i = j = 0; i < 8; i++, j += 8) {
		t2 = C->state[i];
		digest[j+7] = (char)t2 & 0xff; t2>>=8;
		digest[j+6] = (char)t2 & 0xff; t2>>=8;
		digest[j+5] = (char)t2 & 0xff; t2>>=8;
		digest[j+4] = (char)t2 & 0xff; t2>>=8;
		digest[j+3] = (char)t2 & 0xff; t2>>=8;
		digest[j+2] = (char)t2 & 0xff; t2>>=8;
		digest[j+1] = (char)t2 & 0xff; t2>>=8;
		digest[j  ] = (char)t2 & 0xff;
	}
	
	/* Zeroize sensitive information. */
	memset(C, 0, sizeof(struct SHA512_CTX));
}

static int
sha512_open(struct digest_context *cx, int atomic)
{
	if (!cx || !cx->digest_info)
		return -EINVAL;

	SHA512Init((struct SHA512_CTX *) cx->digest_info);

	return 0;
}

static int
sha512_update(struct digest_context *cx, const u8 *in, int size, int atomic)
{
	if (!cx || !in || !cx->digest_info)
		return -EINVAL;

	SHA512Update((struct SHA512_CTX *) cx->digest_info, in, size);

	return 0;
}

static int
sha512_digest(struct digest_context *cx, u8 *out, int atomic)
{
	struct SHA512_CTX tmp;

	if (!cx || !out || !cx->digest_info)
		return -EINVAL;

	memcpy(&tmp, (struct SHA512_CTX *)cx->digest_info,
	       sizeof (struct SHA512_CTX));
	SHA512Final(&tmp, out);

	return 0;
}

static int
sha512_close(struct digest_context *cx, u8 *out, int atomic)
{
	static u8 tmp[64];

	if (!cx || !cx->digest_info)
		return -EINVAL;
	
	if (out == 0)
		out = tmp;

	SHA512Final((struct SHA512_CTX *)cx->digest_info, out);

	return 0;
}

#define DIGEST_ID		sha512
#define DIGEST_SIZE		sizeof (struct SHA512_CTX)
#define DIGEST_BLOCKSIZE	64

#include "gen-hash.h"

EXPORT_NO_SYMBOLS;
