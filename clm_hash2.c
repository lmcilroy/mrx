#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <x86intrin.h>

#include "clm_hash2.h"

static const __m128i k1 = { 0xc3a5c85c97cb3127, 0xb492b66fbe98f273 };

static const __m128i s1 = { 0x9ae16a3b2f90404f, 0x9ae16a3b2f90404f };
static const __m128i s2 = { 0xc949d7c7509e6557, 0xc949d7c7509e6557 };
static const __m128i s3 = { 0xff51afd7ed558ccd, 0xff51afd7ed558ccd };
static const __m128i s4 = { 0xc4ceb9fe1a85ec53, 0xc4ceb9fe1a85ec53 };
static const __m128i s5 = { 0xc05bbd6b47c57574, 0xf83c1e9e4a934534 };
static const __m128i s6 = { 0xa224e4507e645d91, 0xe7a9131a4b842813 };
static const __m128i s7 = { 0x58f082e3580f347e, 0xceb5795349196aff };
static const __m128i s8 = { 0xb549200b5168588f, 0xfd5c07200540ada5 };

static inline void
mix1024(__m128i * const h1, __m128i * const h2, __m128i * const h3,
    __m128i * const h4, __m128i * const h5, __m128i * const h6,
    __m128i * const h7, __m128i * const h8)
{
	__m128i x1, x2, x3, x4, x5, x6, x7, x8;

	x1 = _mm_xor_si128(*h1, k1);
	x2 = _mm_xor_si128(*h2, k1);
	x3 = _mm_xor_si128(*h3, k1);
	x4 = _mm_xor_si128(*h4, k1);
	x5 = _mm_xor_si128(*h5, k1);
	x6 = _mm_xor_si128(*h6, k1);
	x7 = _mm_xor_si128(*h7, k1);
	x8 = _mm_xor_si128(*h8, k1);

	*h1 = _mm_clmulepi64_si128(x1, x2, 0x10);
	*h2 = _mm_clmulepi64_si128(x2, x3, 0x10);
	*h3 = _mm_clmulepi64_si128(x3, x4, 0x10);
	*h4 = _mm_clmulepi64_si128(x4, x5, 0x10);
	*h5 = _mm_clmulepi64_si128(x5, x6, 0x10);
	*h6 = _mm_clmulepi64_si128(x6, x7, 0x10);
	*h7 = _mm_clmulepi64_si128(x7, x8, 0x10);
	*h8 = _mm_clmulepi64_si128(x8, x1, 0x10);
}

static inline __m128i
final128(const __m128i h)
{
	__m128i a = h;

	a = _mm_xor_si128(a, _mm_srli_epi32(a, 16));
	a = _mm_mullo_epi32(a, k1);
	a = _mm_xor_si128(a, _mm_srli_epi32(a, 16));
	a = _mm_mullo_epi32(a, s1);
	a = _mm_xor_si128(a, _mm_srli_epi32(a, 16));

	return a;
}

void
clm_hash2_start_seed(clm_hash2_state_t * const state, const void * const seed)
{
	__m128i s;

	s = _mm_loadu_si128(seed);
	state->x1 = _mm_xor_si128(s1, s);
	state->x2 = _mm_xor_si128(s2, s);
	state->x3 = _mm_xor_si128(s3, s);
	state->x4 = _mm_xor_si128(s4, s);
	state->x5 = _mm_xor_si128(s5, s);
	state->x6 = _mm_xor_si128(s6, s);
	state->x7 = _mm_xor_si128(s7, s);
	state->x8 = _mm_xor_si128(s8, s);
	state->block_size = sizeof(state->buf);
	state->count = 0;
	state->total_len = 0;
}

void
clm_hash2_start(clm_hash2_state_t * const state)
{
	uint64_t seed[2] = { 0, 0 };

	clm_hash2_start_seed(state, seed);
}

void
clm_hash2_update(clm_hash2_state_t * const state, const void * const data,
    const size_t len)
{
	__m128i *v;
	__m128i x1, x2, x3, x4, x5, x6, x7, x8;
	const unsigned char *curr = data;
	size_t llen;
	uint32_t left;

	if (data == NULL || len == 0)
		return;

	state->total_len += len;

	if (state->count + len < state->block_size) {
		memcpy(state->buf.chars + state->count, data, len);
		state->count += len;
		return;
	}

	llen = len;

	if (state->count > 0) {
		left = state->block_size - state->count;
		memcpy(state->buf.chars + state->count, data, left);
		llen -= left;
		curr += left;
	}

	x1 = state->x1;
	x2 = state->x2;
	x3 = state->x3;
	x4 = state->x4;
	x5 = state->x5;
	x6 = state->x6;
	x7 = state->x7;
	x8 = state->x8;

	if (state->count > 0) {
		v = state->buf.ints;

		x1 = _mm_xor_si128(x1, v[0]);
		x2 = _mm_xor_si128(x2, v[1]);
		x3 = _mm_xor_si128(x3, v[2]);
		x4 = _mm_xor_si128(x4, v[3]);
		x5 = _mm_xor_si128(x5, v[4]);
		x6 = _mm_xor_si128(x6, v[5]);
		x7 = _mm_xor_si128(x7, v[6]);
		x8 = _mm_xor_si128(x8, v[7]);
		mix1024(&x1, &x2, &x3, &x4, &x5, &x6, &x7, &x8);

		state->count = 0;
	}

	while (llen >= state->block_size) {
		v = (__m128i_u *)curr;

		x1 = _mm_xor_si128(x1, _mm_loadu_si128(&v[0]));
		x2 = _mm_xor_si128(x2, _mm_loadu_si128(&v[1]));
		x3 = _mm_xor_si128(x3, _mm_loadu_si128(&v[2]));
		x4 = _mm_xor_si128(x4, _mm_loadu_si128(&v[3]));
		x5 = _mm_xor_si128(x5, _mm_loadu_si128(&v[4]));
		x6 = _mm_xor_si128(x6, _mm_loadu_si128(&v[5]));
		x7 = _mm_xor_si128(x7, _mm_loadu_si128(&v[6]));
		x8 = _mm_xor_si128(x8, _mm_loadu_si128(&v[7]));
		mix1024(&x1, &x2, &x3, &x4, &x5, &x6, &x7, &x8);

		curr += state->block_size;
		llen -= state->block_size;
	}

	state->x1 = x1;
	state->x2 = x2;
	state->x3 = x3;
	state->x4 = x4;
	state->x5 = x5;
	state->x6 = x6;
	state->x7 = x7;
	state->x8 = x8;

	if (llen > 0) {
		memcpy(state->buf.chars, curr, llen);
		state->count = llen;
	}
}

static const unsigned char padding[256] = { 0x80 };

void
clm_hash2_end(clm_hash2_state_t * const state, clm_hash2_t * const hash)
{
	__m128i x1, x2, x3, x4, x5, x6, x7, x8;
	__m128i h[8];
	uint64_t total_len;
	uint32_t len, i;

	total_len = state->total_len;
	len = (state->count < 120) ? 120 - state->count : 248 - state->count;
	clm_hash2_update(state, padding, len);
	clm_hash2_update(state, &total_len, 8);

	x1 = state->x1;
	x2 = state->x2;
	x3 = state->x3;
	x4 = state->x4;
	x5 = state->x5;
	x6 = state->x6;
	x7 = state->x7;
	x8 = state->x8;

	for (i = 0; i < 16; i++)
		mix1024(&x1, &x2, &x3, &x4, &x5, &x6, &x7, &x8);

	h[0] = final128(x1);
	h[1] = final128(x2);
	h[2] = final128(x3);
	h[3] = final128(x4);
	h[4] = final128(x5);
	h[5] = final128(x6);
	h[6] = final128(x7);
	h[7] = final128(x8);

	memcpy(hash, h, sizeof(h));
}

void
clm_hash2_seed(const void * const data, const size_t len,
    const void * const seed, clm_hash2_t * const hash)
{
	clm_hash2_state_t clm_hash;

	clm_hash2_start_seed(&clm_hash, seed);
	clm_hash2_update(&clm_hash, data, len);
	clm_hash2_end(&clm_hash, hash);
}

void
clm_hash2(const void * const data, const size_t len, clm_hash2_t * const hash)
{
	uint64_t seed[2] = { 0, 0 };

	clm_hash2_seed(data, len, seed, hash);
}
