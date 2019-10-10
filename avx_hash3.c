#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <x86intrin.h>

#include "avx_hash3.h"

static const __m256i m1 = { 0xcc9e2d51db873593, 0xcc9e2d51db873593,
			    0xcc9e2d51db873593, 0xcc9e2d51db873593 };

static const __m256i s1 = { 0xc05bbd6b47c57574, 0xf83c1e9e4a934534,
			    0xc05bbd6b47c57574, 0xf83c1e9e4a934534 };
static const __m256i s2 = { 0xa224e4507e645d91, 0xe7a9131a4b842813,
			    0xa224e4507e645d91, 0xe7a9131a4b842813 };
static const __m256i s3 = { 0x58f082e3580f347e, 0xceb5795349196aff,
			    0x58f082e3580f347e, 0xceb5795349196aff };
static const __m256i s4 = { 0xb549200b5168588f, 0xfd5c07200540ada5,
			    0xb549200b5168588f, 0xfd5c07200540ada5 };
static const __m256i s5 = { 0x526b8e90108029d5, 0x46e634784c1350f0,
			    0x526b8e90108029d5, 0x46e634784c1350f0 };
static const __m256i s6 = { 0x4e9a8c21921fc259, 0x8f5d6730b44e65c3,
			    0x4e9a8c21921fc259, 0x8f5d6730b44e65c3 };
static const __m256i s7 = { 0xac0120817de73b56, 0xa5be99de5ff769dd,
			    0xac0120817de73b56, 0xa5be99de5ff769dd };
static const __m256i s8 = { 0x7541811329b4609f, 0x4c4cfa866064ec93,
			    0x7541811329b4609f, 0x4c4cfa866064ec93 };

static inline __m256i
rotate256(const __m256i h)
{
	__m256i a;

	a = _mm256_set_epi32(6, 5, 4, 3, 2, 1, 0, 7);
	a = _mm256_permutevar8x32_epi32(h, a);

	return a;
}

static inline __m256i
mix256(const __m256i h)
{
	__m256i a, b;

	a = _mm256_xor_si256(h, m1);
	b = rotate256(a);
	a = _mm256_mul_epu32(a, b);

	return a;
}

static inline __m256i
round256(const __m256i h, const __m256i x)
{
	__m256i a;

	a = _mm256_xor_si256(h, x);
	a = mix256(a);

	return a;
}

static inline __m256i
fmix256(const __m256i h)
{
	__m256i a;

	a = _mm256_xor_si256(h, _mm256_srli_epi32(h, 16));
	a = _mm256_mullo_epi32(a, m1);
	a = _mm256_xor_si256(a, _mm256_srli_epi32(a, 16));

	return a;
}

static inline __m256i
combine256(const __m256i h, const __m256i x)
{
	__m256i a, m;

	m = _mm256_set_epi32(3, 3, 3, 3, 3, 3, 3, 3);
	a = _mm256_mullo_epi32(h, m);
	a = _mm256_add_epi64(a, x);

	return a;
}

void
avx_hash3_start_seed(avx_hash3_state_t * const state, const void * const seed)
{
	__m256i s;

	s = _mm256_loadu_si256(seed);
	state->x1 = _mm256_xor_si256(s1, s);
	state->x2 = _mm256_xor_si256(s2, s);
	state->x3 = _mm256_xor_si256(s3, s);
	state->x4 = _mm256_xor_si256(s4, s);
	state->x5 = _mm256_xor_si256(s5, s);
	state->x6 = _mm256_xor_si256(s6, s);
	state->x7 = _mm256_xor_si256(s7, s);
	state->x8 = _mm256_xor_si256(s8, s);
	state->block_size = sizeof(state->buf);
	state->count = 0;
	state->total_len = 0;
}

void
avx_hash3_start(avx_hash3_state_t * const state)
{
	uint64_t seed[4] = { 0, 0, 0, 0 };

	avx_hash3_start_seed(state, &seed);
}

void
avx_hash3_update(avx_hash3_state_t * const state, const void * const data,
    const size_t len)
{
	__m256i *v;
	__m256i x1, x2, x3, x4, x5, x6, x7, x8;
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

		x1 = round256(x1, v[0]);
		x2 = round256(x2, v[1]);
		x3 = round256(x3, v[2]);
		x4 = round256(x4, v[3]);
		x5 = round256(x5, v[4]);
		x6 = round256(x6, v[5]);
		x7 = round256(x7, v[6]);
		x8 = round256(x8, v[7]);

		state->count = 0;
	}

	while (llen >= state->block_size) {
		v = (__m256i *)curr;

		x1 = round256(x1, _mm256_loadu_si256(&v[0]));
		x2 = round256(x2, _mm256_loadu_si256(&v[1]));
		x3 = round256(x3, _mm256_loadu_si256(&v[2]));
		x4 = round256(x4, _mm256_loadu_si256(&v[3]));
		x5 = round256(x5, _mm256_loadu_si256(&v[4]));
		x6 = round256(x6, _mm256_loadu_si256(&v[5]));
		x7 = round256(x7, _mm256_loadu_si256(&v[6]));
		x8 = round256(x8, _mm256_loadu_si256(&v[7]));

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
avx_hash3_end(avx_hash3_state_t * const state, avx_hash3_t * const hash)
{
	__m256i h, x1, x2, x3, x4, x5, x6, x7, x8;
	uint64_t total_len;
	uint32_t len, i;

	total_len = state->total_len;
	len = (state->count < 248) ? 248 - state->count : 504 - state->count;
	avx_hash3_update(state, padding, len);
	avx_hash3_update(state, &total_len, 8);

	x1 = state->x1;
	x2 = state->x2;
	x3 = state->x3;
	x4 = state->x4;
	x5 = state->x5;
	x6 = state->x6;
	x7 = state->x7;
	x8 = state->x8;

	for (i = 0; i < 8; i++) {
		x1 = mix256(x1);
		x2 = mix256(x2);
		x3 = mix256(x3);
		x4 = mix256(x4);
		x5 = mix256(x5);
		x6 = mix256(x6);
		x7 = mix256(x7);
		x8 = mix256(x8);
	}

	x1 = fmix256(x1);
	x2 = fmix256(x2);
	x3 = fmix256(x3);
	x4 = fmix256(x4);
	x5 = fmix256(x5);
	x6 = fmix256(x6);
	x7 = fmix256(x7);
	x8 = fmix256(x8);

	h = _mm256_setzero_si256();
	h = combine256(h, x1);
	h = combine256(h, x2);
	h = combine256(h, x3);
	h = combine256(h, x4);
	h = combine256(h, x5);
	h = combine256(h, x6);
	h = combine256(h, x7);
	h = combine256(h, x8);

	_mm256_storeu_si256((__m256i *)hash, h);
}

void
avx_hash3_seed(const void * const data, const size_t len,
    const void * const seed, avx_hash3_t * const hash)
{
	avx_hash3_state_t avx_hash;

	avx_hash3_start_seed(&avx_hash, seed);
	avx_hash3_update(&avx_hash, data, len);
	avx_hash3_end(&avx_hash, hash);
}

void
avx_hash3(const void * const data, const size_t len, avx_hash3_t * const hash)
{
	uint64_t seed[4] = { 0, 0, 0, 0 };

	avx_hash3_seed(data, len, &seed, hash);
}
