#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <x86intrin.h>

#include "clm_hash.h"

static const __m128i k1 = { 0xc3a5c85c97cb3127, 0xb492b66fbe98f273 };

static const __m128i s1 = { 0x9ae16a3b2f90404f, 0x9ae16a3b2f90404f };
static const __m128i s2 = { 0xc949d7c7509e6557, 0xc949d7c7509e6557 };
static const __m128i s3 = { 0xff51afd7ed558ccd, 0xff51afd7ed558ccd };
static const __m128i s4 = { 0xc4ceb9fe1a85ec53, 0xc4ceb9fe1a85ec53 };
static const __m128i s5 = { 0xc05bbd6b47c57574, 0xf83c1e9e4a934534 };
static const __m128i s6 = { 0xa224e4507e645d91, 0xe7a9131a4b842813 };
static const __m128i s7 = { 0x58f082e3580f347e, 0xceb5795349196aff };
static const __m128i s8 = { 0xb549200b5168588f, 0xfd5c07200540ada5 };

static inline __m128i
mix128(const __m128i h)
{
	__m128i a;

	a = _mm_xor_si128(h, k1);
	a = _mm_clmulepi64_si128(a, a, 0x10);

	return a;
}

static inline __m128i
round128(const __m128i h, const __m128i x)
{
	return mix128(_mm_xor_si128(h, x));
}

static inline __m128i
final128(const __m128i h)
{
	__m128i a;

	a = mix128(h);

	a = _mm_xor_si128(a, _mm_srli_epi32(a, 16));
	a = _mm_mullo_epi32(a, k1);
	a = _mm_xor_si128(a, _mm_srli_epi32(a, 16));
	a = _mm_mullo_epi32(a, s1);
	a = _mm_xor_si128(a, _mm_srli_epi32(a, 16));

	return a;
}

void
clm_hash_start_seed(clm_hash_state_t * const state, const void * const seed)
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
clm_hash_start(clm_hash_state_t * const state)
{
	uint64_t seed[2] = { 0, 0 };

	clm_hash_start_seed(state, &seed);
}

void
clm_hash_update(clm_hash_state_t * const state, const void * const data,
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

		x1 = round128(x1, v[0]);
		x2 = round128(x2, v[1]);
		x3 = round128(x3, v[2]);
		x4 = round128(x4, v[3]);
		x5 = round128(x5, v[4]);
		x6 = round128(x6, v[5]);
		x7 = round128(x7, v[6]);
		x8 = round128(x8, v[7]);

		state->count = 0;
	}

	while (llen >= state->block_size) {
		v = (__m128i_u *)curr;

		x1 = round128(x1, _mm_loadu_si128(&v[0]));
		x2 = round128(x2, _mm_loadu_si128(&v[1]));
		x3 = round128(x3, _mm_loadu_si128(&v[2]));
		x4 = round128(x4, _mm_loadu_si128(&v[3]));
		x5 = round128(x5, _mm_loadu_si128(&v[4]));
		x6 = round128(x6, _mm_loadu_si128(&v[5]));
		x7 = round128(x7, _mm_loadu_si128(&v[6]));
		x8 = round128(x8, _mm_loadu_si128(&v[7]));

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
clm_hash_end(clm_hash_state_t * const state, clm_hash_t * const hash)
{
	__m128i h, x1, x2, x3, x4, x5, x6, x7, x8;
	uint64_t total_len;
	uint32_t len;

	total_len = state->total_len;
	len = (state->count < 120) ? 120 - state->count : 248 - state->count;
	clm_hash_update(state, padding, len);
	clm_hash_update(state, &total_len, 8);

	x1 = final128(state->x1);
	x2 = final128(state->x2);
	x3 = final128(state->x3);
	x4 = final128(state->x4);
	x5 = final128(state->x5);
	x6 = final128(state->x6);
	x7 = final128(state->x7);
	x8 = final128(state->x8);

	/* Combine the streams into one hash */
	h = _mm_setzero_si128();
	h = round128(h, x1);
	h = round128(h, x2);
	h = round128(h, x3);
	h = round128(h, x4);
	h = round128(h, x5);
	h = round128(h, x6);
	h = round128(h, x7);
	h = round128(h, x8);
	h = final128(h);

	_mm_storeu_si128((__m128i_u *)hash, h);
}

void
clm_hash_seed(const void * const data, const size_t len,
    const void * const seed, clm_hash_t * const hash)
{
	clm_hash_state_t clm_hash;

	clm_hash_start_seed(&clm_hash, seed);
	clm_hash_update(&clm_hash, data, len);
	clm_hash_end(&clm_hash, hash);
}

void
clm_hash(const void * const data, const size_t len, clm_hash_t * const hash)
{
	uint64_t seed[2] = { 0, 0 };

	clm_hash_seed(data, len, &seed, hash);
}
