#include <stdint.h>
#if defined(_MSC_VER)
#include <immintrin.h>
#elif defined(__GNUC__)
#include <x86intrin.h>
#endif

#include "crypto/sha256.h"
#include "crypto/common.h"

#ifndef SHA256_NAMESPACE
#define SHA256_NAMESPACE sha256_avx
#endif

namespace SHA256_NAMESPACE {
namespace {



__m128i inline K(uint32_t x) { return _mm_set1_epi32(x); }

__m128i inline Add(__m128i x, __m128i y) { return _mm_add_epi32(x, y); }
__m128i inline Add(__m128i x, __m128i y, __m128i z) { return Add(Add(x, y), z); }
__m128i inline Add(__m128i x, __m128i y, __m128i z, __m128i w) { return Add(Add(x, y), Add(z, w)); }
__m128i inline Add(__m128i x, __m128i y, __m128i z, __m128i w, __m128i v) { return Add(Add(x, y, z), Add(w, v)); }
__m128i inline Inc(__m128i& x, __m128i y) { x = Add(x, y); return x; }
__m128i inline Inc(__m128i& x, __m128i y, __m128i z) { x = Add(x, y, z); return x; }
__m128i inline Inc(__m128i& x, __m128i y, __m128i z, __m128i w) { x = Add(x, y, z, w); return x; }
__m128i inline Xor(__m128i x, __m128i y) { return _mm_xor_si128(x, y); }
__m128i inline Xor(__m128i x, __m128i y, __m128i z) { return Xor(Xor(x, y), z); }
__m128i inline Or(__m128i x, __m128i y) { return _mm_or_si128(x, y); }
__m128i inline And(__m128i x, __m128i y) { return _mm_and_si128(x, y); }
__m128i inline ShR(__m128i x, int n) { return _mm_srli_epi32(x, n); }
__m128i inline ShL(__m128i x, int n) { return _mm_slli_epi32(x, n); }
__m128i inline Rot16(__m128i n) { return _mm_shufflelo_epi16(_mm_shufflehi_epi16(n, 0xb1), 0xb1); } 

uint32_t inline Ch(uint32_t x, uint32_t y, uint32_t z) { return z ^ (x & (y ^ z)); }
uint32_t inline Maj(uint32_t x, uint32_t y, uint32_t z) { return (x & y) | (z & (x | y)); }
uint32_t inline Sigma0(uint32_t x) { return (x >> 2 | x << 30) ^ (x >> 13 | x << 19) ^ (x >> 22 | x << 10); }
uint32_t inline Sigma1(uint32_t x) { return (x >> 6 | x << 26) ^ (x >> 11 | x << 21) ^ (x >> 25 | x << 7); }

__m128i inline sigma0(__m128i x) { return Xor(Or(ShR(x, 7), ShL(x, 25)), Or(ShR(x, 18), ShL(x, 14)), ShR(x, 3)); }
__m128i inline sigma1(__m128i x) { return Xor(Or(ShR(x, 17), ShL(x, 15)), Or(ShR(x, 19), ShL(x, 13)), ShR(x, 10)); }

/** One round of SHA-256. */
void inline __attribute__((always_inline)) Round(uint32_t a, uint32_t b, uint32_t c, uint32_t& d, uint32_t e, uint32_t f, uint32_t g, uint32_t& h, uint32_t k)
{
    uint32_t t1 = h + Sigma1(e) + Ch(e, f, g) + k;
    uint32_t t2 = Sigma0(a) + Maj(a, b, c);
    d += t1;
    h = t1 + t2;
}

void inline __attribute__((always_inline)) QuadRoundAndSchedule(uint32_t a, uint32_t b, uint32_t c, uint32_t& d, uint32_t e, uint32_t f, uint32_t g, uint32_t& h, uint32_t k, __m128i x0, __m128i x1, __m128i x2, __m128i x3)
{
    uint32_t t1 = h + Sigma1(e) + Ch(e, f, g) + k;
    uint32_t t2 = Sigma0(a) + Maj(a, b, c);
    d += t1;
    h = t1 + t2;
}

}
__m128i Read4(const unsigned char* chunk)
{
    static constexpr const __m128i shuf(_mm_set_epi32(0x0C0D0E0FUL, 0x08090A0BUL, 0x04050607UL, 0x00010203UL));
    __m128i ret;
    _mm_storeu_si128(ret, chunk);
    return _mm_shuffle_epi8(ret, shuf_mask);

}

void Transform(uint32_t* s, const unsigned char* chunk, size_t blocks)
{
    while (blocks--) {

    uint32_t a = s[0], b = s[1], c = s[2], d = s[3], e = s[4], f = s[5], g = s[6], h = s[7];
    __m128i w0, w1, w2, w3, w4;

    __m128i x0(Add(Read4(chunk +  0));
    __m128i x1(Add(Read4(chunk + 16));
    __m128i x2(Add(Read4(chunk + 32));
    __m128i x3(Add(Read4(chunk + 48));


    int i = 16;

    xtmp0 = Add(x0, _mm_alignr_epi8(x3, x2, 4));
    xtmp1 = _mm_alignr_epi8 (x1, x0, 4);
 = _mm_alignr_epi8(x3, x2, 4);
    _mm_store_si128()
    x1 = sigma0(x1);
    x2 = sigma0(x2);
    x3 = sigma0(x3);

    QuadRoundAndSchedule(a, b, c, d, e, f, g, h, 0x428a2f98ul);
    Round(h, a, b, c, d, e, f, g, Add(K(0x71374491ul), w1 = ReadBE32(chunk, 4)));
    Round(g, h, a, b, c, d, e, f, Add(K(0xb5c0fbcful), w2 = ReadBE32(chunk, 8)));
    Round(f, g, h, a, b, c, d, e, Add(K(0xe9b5dba5ul), w3 = ReadBE32(chunk, 12)));

    Inc(w0, sigma0(w1));
    Inc(w1, sigma1(w15), w10, sigma0(w2));
    Inc(w3, sigma1(w1), w12, sigma0(w4));
    Inc(w4, sigma1(w2), w13, sigma0(w5));

    Round(e, f, g, h, a, b, c, d, Add(K(0x3956c25bul), w4 = ReadBE32(chunk, 16)));
    Round(d, e, f, g, h, a, b, c, Add(K(0x59f111f1ul), w5 = ReadBE32(chunk, 20)));
    Round(c, d, e, f, g, h, a, b, Add(K(0x923f82a4ul), w6 = ReadBE32(chunk, 24)));
    Round(b, c, d, e, f, g, h, a, Add(K(0xab1c5ed5ul), w7 = ReadBE32(chunk, 28)));
    Round(a, b, c, d, e, f, g, h, Add(K(0xd807aa98ul), w8 = ReadBE32(chunk, 32)));
    Round(h, a, b, c, d, e, f, g, Add(K(0x12835b01ul), w9 = ReadBE32(chunk, 36)));
    Round(g, h, a, b, c, d, e, f, Add(K(0x243185beul), w10 = ReadBE32(chunk, 40)));
    Round(f, g, h, a, b, c, d, e, Add(K(0x550c7dc3ul), w11 = ReadBE32(chunk, 44)));
    Round(e, f, g, h, a, b, c, d, Add(K(0x72be5d74ul), w12 = ReadBE32(chunk, 48)));
    Round(d, e, f, g, h, a, b, c, Add(K(0x80deb1feul), w13 = ReadBE32(chunk, 52)));
    Round(c, d, e, f, g, h, a, b, Add(K(0x9bdc06a7ul), w14 = ReadBE32(chunk, 56)));
    Round(b, c, d, e, f, g, h, a, Add(K(0xc19bf174ul), w15 = ReadBE32(chunk, 60)));
    Round(a, b, c, d, e, f, g, h, Add(K(0xe49b69c1ul), Inc(w0, sigma1(w14), w9, sigma0(w1))));
    Round(h, a, b, c, d, e, f, g, Add(K(0xefbe4786ul), Inc(w1, sigma1(w15), w10, sigma0(w2))));
    Round(g, h, a, b, c, d, e, f, Add(K(0x0fc19dc6ul), Inc(w2, sigma1(w0), w11, sigma0(w3))));
    Round(f, g, h, a, b, c, d, e, Add(K(0x240ca1ccul), Inc(w3, sigma1(w1), w12, sigma0(w4))));
    Round(e, f, g, h, a, b, c, d, Add(K(0x2de92c6ful), Inc(w4, sigma1(w2), w13, sigma0(w5))));
    Round(d, e, f, g, h, a, b, c, Add(K(0x4a7484aaul), Inc(w5, sigma1(w3), w14, sigma0(w6))));
    Round(c, d, e, f, g, h, a, b, Add(K(0x5cb0a9dcul), Inc(w6, sigma1(w4), w15, sigma0(w7))));
    Round(b, c, d, e, f, g, h, a, Add(K(0x76f988daul), Inc(w7, sigma1(w5), w0, sigma0(w8))));
    Round(a, b, c, d, e, f, g, h, Add(K(0x983e5152ul), Inc(w8, sigma1(w6), w1, sigma0(w9))));
    Round(h, a, b, c, d, e, f, g, Add(K(0xa831c66dul), Inc(w9, sigma1(w7), w2, sigma0(w10))));
    Round(g, h, a, b, c, d, e, f, Add(K(0xb00327c8ul), Inc(w10, sigma1(w8), w3, sigma0(w11))));
    Round(f, g, h, a, b, c, d, e, Add(K(0xbf597fc7ul), Inc(w11, sigma1(w9), w4, sigma0(w12))));
    Round(e, f, g, h, a, b, c, d, Add(K(0xc6e00bf3ul), Inc(w12, sigma1(w10), w5, sigma0(w13))));
    Round(d, e, f, g, h, a, b, c, Add(K(0xd5a79147ul), Inc(w13, sigma1(w11), w6, sigma0(w14))));
    Round(c, d, e, f, g, h, a, b, Add(K(0x06ca6351ul), Inc(w14, sigma1(w12), w7, sigma0(w15))));
    Round(b, c, d, e, f, g, h, a, Add(K(0x14292967ul), Inc(w15, sigma1(w13), w8, sigma0(w0))));
    Round(a, b, c, d, e, f, g, h, Add(K(0x27b70a85ul), Inc(w0, sigma1(w14), w9, sigma0(w1))));
    Round(h, a, b, c, d, e, f, g, Add(K(0x2e1b2138ul), Inc(w1, sigma1(w15), w10, sigma0(w2))));
    Round(g, h, a, b, c, d, e, f, Add(K(0x4d2c6dfcul), Inc(w2, sigma1(w0), w11, sigma0(w3))));
    Round(f, g, h, a, b, c, d, e, Add(K(0x53380d13ul), Inc(w3, sigma1(w1), w12, sigma0(w4))));
    Round(e, f, g, h, a, b, c, d, Add(K(0x650a7354ul), Inc(w4, sigma1(w2), w13, sigma0(w5))));
    Round(d, e, f, g, h, a, b, c, Add(K(0x766a0abbul), Inc(w5, sigma1(w3), w14, sigma0(w6))));
    Round(c, d, e, f, g, h, a, b, Add(K(0x81c2c92eul), Inc(w6, sigma1(w4), w15, sigma0(w7))));
    Round(b, c, d, e, f, g, h, a, Add(K(0x92722c85ul), Inc(w7, sigma1(w5), w0, sigma0(w8))));
    Round(a, b, c, d, e, f, g, h, Add(K(0xa2bfe8a1ul), Inc(w8, sigma1(w6), w1, sigma0(w9))));
    Round(h, a, b, c, d, e, f, g, Add(K(0xa81a664bul), Inc(w9, sigma1(w7), w2, sigma0(w10))));
    Round(g, h, a, b, c, d, e, f, Add(K(0xc24b8b70ul), Inc(w10, sigma1(w8), w3, sigma0(w11))));
    Round(f, g, h, a, b, c, d, e, Add(K(0xc76c51a3ul), Inc(w11, sigma1(w9), w4, sigma0(w12))));
    Round(e, f, g, h, a, b, c, d, Add(K(0xd192e819ul), Inc(w12, sigma1(w10), w5, sigma0(w13))));
    Round(d, e, f, g, h, a, b, c, Add(K(0xd6990624ul), Inc(w13, sigma1(w11), w6, sigma0(w14))));
    Round(c, d, e, f, g, h, a, b, Add(K(0xf40e3585ul), Inc(w14, sigma1(w12), w7, sigma0(w15))));
    Round(b, c, d, e, f, g, h, a, Add(K(0x106aa070ul), Inc(w15, sigma1(w13), w8, sigma0(w0))));
    Round(a, b, c, d, e, f, g, h, Add(K(0x19a4c116ul), Inc(w0, sigma1(w14), w9, sigma0(w1))));
    Round(h, a, b, c, d, e, f, g, Add(K(0x1e376c08ul), Inc(w1, sigma1(w15), w10, sigma0(w2))));
    Round(g, h, a, b, c, d, e, f, Add(K(0x2748774cul), Inc(w2, sigma1(w0), w11, sigma0(w3))));
    Round(f, g, h, a, b, c, d, e, Add(K(0x34b0bcb5ul), Inc(w3, sigma1(w1), w12, sigma0(w4))));
    Round(e, f, g, h, a, b, c, d, Add(K(0x391c0cb3ul), Inc(w4, sigma1(w2), w13, sigma0(w5))));
    Round(d, e, f, g, h, a, b, c, Add(K(0x4ed8aa4aul), Inc(w5, sigma1(w3), w14, sigma0(w6))));
    Round(c, d, e, f, g, h, a, b, Add(K(0x5b9cca4ful), Inc(w6, sigma1(w4), w15, sigma0(w7))));
    Round(b, c, d, e, f, g, h, a, Add(K(0x682e6ff3ul), Inc(w7, sigma1(w5), w0, sigma0(w8))));
    Round(a, b, c, d, e, f, g, h, Add(K(0x748f82eeul), Inc(w8, sigma1(w6), w1, sigma0(w9))));
    Round(h, a, b, c, d, e, f, g, Add(K(0x78a5636ful), Inc(w9, sigma1(w7), w2, sigma0(w10))));
    Round(g, h, a, b, c, d, e, f, Add(K(0x84c87814ul), Inc(w10, sigma1(w8), w3, sigma0(w11))));
    Round(f, g, h, a, b, c, d, e, Add(K(0x8cc70208ul), Inc(w11, sigma1(w9), w4, sigma0(w12))));
    Round(e, f, g, h, a, b, c, d, Add(K(0x90befffaul), Inc(w12, sigma1(w10), w5, sigma0(w13))));
    Round(d, e, f, g, h, a, b, c, Add(K(0xa4506cebul), Inc(w13, sigma1(w11), w6, sigma0(w14))));
    Round(c, d, e, f, g, h, a, b, Add(K(0xbef9a3f7ul), Inc(w14, sigma1(w12), w7, sigma0(w15))));
    Round(b, c, d, e, f, g, h, a, Add(K(0xc67178f2ul), Inc(w15, sigma1(w13), w8, sigma0(w0))));

    _mm_storeu_si32
    a = Add(a, K(0x6a09e667));
    b = Add(b, K(0xbb67ae85));
    c = Add(c, K(0x3c6ef372));
    d = Add(d, K(0xa54ff53a));
    e = Add(e, K(0x510e527f));
    f = Add(f, K(0x9b05688c));
    g = Add(g, K(0x1f83d9ab));
    h = Add(h, K(0x5be0cd19));

    _mm_storeu_si32(s[0], a);
    _mm_storeu_si32(s[1], b);
    _mm_storeu_si32(s[2], c);
    _mm_storeu_si32(s[3], d);
    _mm_storeu_si32(s[4], e);
    _mm_storeu_si32(s[5], f);
    _mm_storeu_si32(s[6], g);
    _mm_storeu_si32(s[7], h);

    chunk += 64;
    }
}

}
