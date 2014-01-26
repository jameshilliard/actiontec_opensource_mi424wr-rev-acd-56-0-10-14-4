/* $Id: digest-sha256.c,v 1.1.1.1 2006/01/20 03:16:22 zjjiang Exp $
 *
 * SHA-256 code by Jean-Luc Cooke <jlcooke@certainkey.com>.
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
#include <linux/init.h>
#include <linux/types.h>

#include <asm/uaccess.h>

#include <linux/crypto.h>
#include <linux/wordops.h>

#ifdef MODULE_LICENSE
MODULE_LICENSE("GPL");
#endif /* MODULE_LICENSE */
#ifdef MODULE_DESCRIPTION
MODULE_DESCRIPTION ("SHA256 Digest / CryptoAPI");
#endif

/* Disabling this will reduce the size of the code by a considerable
   amount, that you might be interested in reclaiming if you require
   smaller code. However, this will come at the cost of some speed. */
/* #define CONFIG_DIGEST_FAST */

typedef struct {
	u32 state[8];
	u32 count[2];
	u8 buf[128];
} sha256_ctx_t;

#define Ch(x,y,z)   ((x & y) ^ (~x & z))
#define Maj(x,y,z)  ((x & y) ^ ( x & z) ^ (y & z))
#define RORu32(x,y) generic_rotr32(x, y)
#define e0(x)       (RORu32(x, 2) ^ RORu32(x,13) ^ RORu32(x,22))
#define e1(x)       (RORu32(x, 6) ^ RORu32(x,11) ^ RORu32(x,25))
#define s0(x)       (RORu32(x, 7) ^ RORu32(x,18) ^ (x >> 3))
#define s1(x)       (RORu32(x,17) ^ RORu32(x,19) ^ (x >> 10))

#define H0         0x6a09e667
#define H1         0xbb67ae85
#define H2         0x3c6ef372
#define H3         0xa54ff53a
#define H4         0x510e527f
#define H5         0x9b05688c
#define H6         0x1f83d9ab
#define H7         0x5be0cd19

#if !defined(CONFIG_DIGEST_FAST)
const static u32 sha256_K[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};
#endif /* CONFIG_DIGEST_FAST */

#define LOAD_OP(I)\
 {\
  t1  = input[(4*I)  ] & 0xff; t1<<=8;\
  t1 |= input[(4*I)+1] & 0xff; t1<<=8;\
  t1 |= input[(4*I)+2] & 0xff; t1<<=8;\
  t1 |= input[(4*I)+3] & 0xff;\
  W[I] = t1;\
 }

#define BLEND_OP(I) W[I] = s1(W[I-2]) + W[I-7] + s0(W[I-15]) + W[I-16];

static void
SHA256Transform(u32 *state, const u8 *input)
{
	u32 a, b, c, d, e, f, g, h, t1, t2;
	u32 W[64];

#if !defined(CONFIG_DIGEST_HASH)
	int i;

	/* load the input */
	LOAD_OP( 0);  LOAD_OP( 1);  LOAD_OP( 2);  LOAD_OP( 3);
	LOAD_OP( 4);  LOAD_OP( 5);  LOAD_OP( 6);  LOAD_OP( 7);
	LOAD_OP( 8);  LOAD_OP( 9);  LOAD_OP(10);  LOAD_OP(11);
	LOAD_OP(12);  LOAD_OP(13);  LOAD_OP(14);  LOAD_OP(15);

	/* now blend */
	for (i=16; i<64; i+=8) {
		BLEND_OP(i  );  BLEND_OP(i+1);  BLEND_OP(i+2);  BLEND_OP(i+3);
		BLEND_OP(i+4);  BLEND_OP(i+5);  BLEND_OP(i+6);  BLEND_OP(i+7);
	}

	/* load the state into our registers */
	a=state[0];  b=state[1];  c=state[2];  d=state[3];
	e=state[4];  f=state[5];  g=state[6];  h=state[7];
  
	/* now iterate */
	for (i=0; i<64; i+=8) {
		t1 = h + e1(e) + Ch(e,f,g) + sha256_K[i  ] + W[i  ];
		t2 = e0(a) + Maj(a,b,c);   d+=t1;  h=t1+t2;
		t1 = g + e1(d) + Ch(d,e,f) + sha256_K[i+1] + W[i+1];
		t2 = e0(h) + Maj(h,a,b);   c+=t1;  g=t1+t2;
		t1 = f + e1(c) + Ch(c,d,e) + sha256_K[i+2] + W[i+2];
		t2 = e0(g) + Maj(g,h,a);   b+=t1;  f=t1+t2;
		t1 = e + e1(b) + Ch(b,c,d) + sha256_K[i+3] + W[i+3];
		t2 = e0(f) + Maj(f,g,h);   a+=t1;  e=t1+t2;
		t1 = d + e1(a) + Ch(a,b,c) + sha256_K[i+4] + W[i+4];
		t2 = e0(e) + Maj(e,f,g);   h+=t1;  d=t1+t2;
		t1 = c + e1(h) + Ch(h,a,b) + sha256_K[i+5] + W[i+5];
		t2 = e0(d) + Maj(d,e,f);   g+=t1;  c=t1+t2;
		t1 = b + e1(g) + Ch(g,h,a) + sha256_K[i+6] + W[i+6];
		t2 = e0(c) + Maj(c,d,e);   f+=t1;  b=t1+t2;
		t1 = a + e1(f) + Ch(f,g,h) + sha256_K[i+7] + W[i+7];
		t2 = e0(b) + Maj(b,c,d);   e+=t1;  a=t1+t2;
	}
#else /* CONFIG_DIGEST_FAST */
	/* load the input */
	LOAD_OP( 0);  LOAD_OP( 1);  LOAD_OP( 2);  LOAD_OP( 3);
	LOAD_OP( 4);  LOAD_OP( 5);  LOAD_OP( 6);  LOAD_OP( 7);
	LOAD_OP( 8);  LOAD_OP( 9);  LOAD_OP(10);  LOAD_OP(11);
	LOAD_OP(12);  LOAD_OP(13);  LOAD_OP(14);  LOAD_OP(15);

	/* now blend */
	BLEND_OP(16);  BLEND_OP(17);  BLEND_OP(18);  BLEND_OP(19);
	BLEND_OP(20);  BLEND_OP(21);  BLEND_OP(22);  BLEND_OP(23);
	BLEND_OP(24);  BLEND_OP(25);  BLEND_OP(26);  BLEND_OP(27);
	BLEND_OP(28);  BLEND_OP(29);  BLEND_OP(30);  BLEND_OP(31);
	BLEND_OP(32);  BLEND_OP(33);  BLEND_OP(34);  BLEND_OP(35);
	BLEND_OP(36);  BLEND_OP(37);  BLEND_OP(38);  BLEND_OP(39);
	BLEND_OP(40);  BLEND_OP(41);  BLEND_OP(42);  BLEND_OP(43);
	BLEND_OP(44);  BLEND_OP(45);  BLEND_OP(46);  BLEND_OP(47);
	BLEND_OP(48);  BLEND_OP(49);  BLEND_OP(50);  BLEND_OP(51);
	BLEND_OP(52);  BLEND_OP(53);  BLEND_OP(54);  BLEND_OP(55);
	BLEND_OP(56);  BLEND_OP(57);  BLEND_OP(58);  BLEND_OP(59);
	BLEND_OP(60);  BLEND_OP(61);  BLEND_OP(62);  BLEND_OP(63);
    
	/* load the state into our registers */
	a=state[0];  b=state[1];  c=state[2];  d=state[3];
	e=state[4];  f=state[5];  g=state[6];  h=state[7];

	/* now iterate */
	t1 = h + e1(e) + Ch(e,f,g) + 0x428a2f98 + W[ 0];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0x71374491 + W[ 1];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0xb5c0fbcf + W[ 2];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0xe9b5dba5 + W[ 3];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0x3956c25b + W[ 4];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0x59f111f1 + W[ 5];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0x923f82a4 + W[ 6];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0xab1c5ed5 + W[ 7];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0xd807aa98 + W[ 8];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0x12835b01 + W[ 9];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0x243185be + W[10];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0x550c7dc3 + W[11];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0x72be5d74 + W[12];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0x80deb1fe + W[13];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0x9bdc06a7 + W[14];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0xc19bf174 + W[15];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0xe49b69c1 + W[16];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0xefbe4786 + W[17];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0x0fc19dc6 + W[18];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0x240ca1cc + W[19];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0x2de92c6f + W[20];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0x4a7484aa + W[21];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0x5cb0a9dc + W[22];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0x76f988da + W[23];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0x983e5152 + W[24];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0xa831c66d + W[25];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0xb00327c8 + W[26];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0xbf597fc7 + W[27];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0xc6e00bf3 + W[28];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0xd5a79147 + W[29];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0x06ca6351 + W[30];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0x14292967 + W[31];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0x27b70a85 + W[32];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0x2e1b2138 + W[33];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0x4d2c6dfc + W[34];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0x53380d13 + W[35];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0x650a7354 + W[36];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0x766a0abb + W[37];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0x81c2c92e + W[38];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0x92722c85 + W[39];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0xa2bfe8a1 + W[40];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0xa81a664b + W[41];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0xc24b8b70 + W[42];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0xc76c51a3 + W[43];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0xd192e819 + W[44];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0xd6990624 + W[45];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0xf40e3585 + W[46];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0x106aa070 + W[47];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0x19a4c116 + W[48];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0x1e376c08 + W[49];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0x2748774c + W[50];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0x34b0bcb5 + W[51];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0x391c0cb3 + W[52];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0x4ed8aa4a + W[53];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0x5b9cca4f + W[54];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0x682e6ff3 + W[55];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;

	t1 = h + e1(e) + Ch(e,f,g) + 0x748f82ee + W[56];
	t2 = e0(a) + Maj(a,b,c);    d+=t1;    h=t1+t2;
	t1 = g + e1(d) + Ch(d,e,f) + 0x78a5636f + W[57];
	t2 = e0(h) + Maj(h,a,b);    c+=t1;    g=t1+t2;
	t1 = f + e1(c) + Ch(c,d,e) + 0x84c87814 + W[58];
	t2 = e0(g) + Maj(g,h,a);    b+=t1;    f=t1+t2;
	t1 = e + e1(b) + Ch(b,c,d) + 0x8cc70208 + W[59];
	t2 = e0(f) + Maj(f,g,h);    a+=t1;    e=t1+t2;
	t1 = d + e1(a) + Ch(a,b,c) + 0x90befffa + W[60];
	t2 = e0(e) + Maj(e,f,g);    h+=t1;    d=t1+t2;
	t1 = c + e1(h) + Ch(h,a,b) + 0xa4506ceb + W[61];
	t2 = e0(d) + Maj(d,e,f);    g+=t1;    c=t1+t2;
	t1 = b + e1(g) + Ch(g,h,a) + 0xbef9a3f7 + W[62];
	t2 = e0(c) + Maj(c,d,e);    f+=t1;    b=t1+t2;
	t1 = a + e1(f) + Ch(f,g,h) + 0xc67178f2 + W[63];
	t2 = e0(b) + Maj(b,c,d);    e+=t1;    a=t1+t2;
#endif /* CONFIG_DIGEST_FAST */

	state[0] += a; state[1] += b; state[2] += c; state[3] += d;
	state[4] += e; state[5] += f; state[6] += g; state[7] += h;

	/* clear any sensitive info... */
	a = b = c = d = e = f = g = h = t1 = t2 = 0;
	memset(W, 0, 64 * sizeof(u32));
}

static void
SHA256Init(sha256_ctx_t *C) {
	C->state[0] = H0;
	C->state[1] = H1;
	C->state[2] = H2;
	C->state[3] = H3;
	C->state[4] = H4;
	C->state[5] = H5;
	C->state[6] = H6;
	C->state[7] = H7;
	C->count[0] = C->count[1] = 0;
	memset(C->buf, 0, 128);
}

static void
SHA256Update(sha256_ctx_t *C, const u8 *input, u32 inputLen)
{
	u32 i, index, partLen;

	/* Compute number of bytes mod 128 */
	index = (u32)((C->count[0] >> 3) & 0x3f);

	/* Update number of bits */
	if ((C->count[0] += (inputLen << 3)) < (inputLen << 3)) {
		C->count[1]++;
		C->count[1] += (inputLen >> 29);
	}

	partLen = 64 - index;

	/* Transform as many times as possible. */
	if (inputLen >= partLen) {
		memcpy(&C->buf[index], input, partLen);
		SHA256Transform(C->state, C->buf);

		for (i = partLen; i + 63 < inputLen; i += 64)
			SHA256Transform(C->state, &input[i]);
		index = 0;
	} else {
		i = 0;
	}
	
	/* Buffer remaining input */
	memcpy(&C->buf[index], &input[i], inputLen-i);
}

static void
SHA256Final(sha256_ctx_t *C, u8 *digest)
{
	const static u8 padding[64] = { 0x80, };
	u8 bits[8];
	u32 t, index, padLen;
	int i, j;

	/* Save number of bits */
	t = C->count[0];
	bits[7] = t; t >>= 8;
	bits[6] = t; t >>= 8;
	bits[5] = t; t >>= 8;
	bits[4] = t; 
	t = C->count[1];
	bits[3] = t; t >>= 8;
	bits[2] = t; t >>= 8;
	bits[1] = t; t >>= 8;
	bits[0] = t;

	/* Pad out to 56 mod 64. */
	index = (C->count[0] >> 3) & 0x3f;
	padLen = (index < 56) ? (56 - index) : ((64+56) - index);
	SHA256Update(C, padding, padLen);

	/* Append length (before padding) */
	SHA256Update(C, bits, sizeof(bits));

	/* Store state in digest */
	for (i = j = 0; i < 8; i++, j += 4) {
		t = C->state[i];
		digest[j+3] = t; t >>= 8;
		digest[j+2] = t; t >>= 8;
		digest[j+1] = t; t >>= 8;
		digest[j  ] = t;
	}

	/* Zeroize sensitive information. */
	memset(C, 0, sizeof(sha256_ctx_t));
}

static int
sha256_open(struct digest_context *cx, int atomic)
{
	if (!cx || !cx->digest_info)
		return -EINVAL;

	SHA256Init((sha256_ctx_t *) cx->digest_info);

	return 0;
}

static int
sha256_update(struct digest_context *cx, const u8 *in, int size, int atomic)
{
	if (!cx || !in || !cx->digest_info)
		return -EINVAL;

	SHA256Update((sha256_ctx_t *) cx->digest_info, in, size);

	return 0;
}

static int
sha256_digest(struct digest_context *cx, u8 *out, int atomic)
{
	sha256_ctx_t tmp;

	if (!cx || !out || !cx->digest_info)
		return -EINVAL;

	memcpy (&tmp, (sha256_ctx_t *) cx->digest_info, 
		sizeof (sha256_ctx_t));

	SHA256Final (&tmp, out);

	return 0;
}

static int
sha256_close(struct digest_context *cx, u8 *out, int atomic)
{
	static u8 tmp[32];

	if (!cx || !cx->digest_info)
		return -EINVAL;
	
	if (out == 0)
		out = tmp;

	SHA256Final((sha256_ctx_t *) cx->digest_info, out);

	return 0;
}

#define DIGEST_ID		sha256
#define DIGEST_BLOCKSIZE	32

#include "gen-hash.h"

EXPORT_NO_SYMBOLS;
