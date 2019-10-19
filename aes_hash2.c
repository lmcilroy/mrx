#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <x86intrin.h>

#include "aes_hash2.h"

static const __m128i k1 = { 0xc05bbd6b47c57574, 0xf83c1e9e4a934534 };
static const __m128i k2 = { 0xa224e4507e645d91, 0xe7a9131a4b842813 };
static const __m128i k3 = { 0x58f082e3580f347e, 0xceb5795349196aff };
static const __m128i k4 = { 0xb549200b5168588f, 0xfd5c07200540ada5 };
static const __m128i k5 = { 0x526b8e90108029d5, 0x46e634784c1350f0 };
static const __m128i k6 = { 0x4e9a8c21921fc259, 0x8f5d6730b44e65c3 };
static const __m128i k7 = { 0xac0120817de73b56, 0xa5be99de5ff769dd };
static const __m128i k8 = { 0x7541811329b4609f, 0x4c4cfa866064ec93 };

static inline void
mix1024(__m128i * const h1, __m128i * const h2, __m128i * const h3,
    __m128i * const h4, __m128i * const h5, __m128i * const h6,
    __m128i * const h7, __m128i * const h8)
{
	__m128i x1, x2, x3, x4, x5, x6, x7, x8;

	x1 = *h1;
	x2 = *h2;
	x3 = *h3;
	x4 = *h4;
	x5 = *h5;
	x6 = *h6;
	x7 = *h7;
	x8 = *h8;

	*h1 = _mm_aesenc_si128(x1, x8);
	*h2 = _mm_aesenc_si128(x2, x1);
	*h3 = _mm_aesenc_si128(x3, x2);
	*h4 = _mm_aesenc_si128(x4, x3);
	*h5 = _mm_aesenc_si128(x5, x4);
	*h6 = _mm_aesenc_si128(x6, x5);
	*h7 = _mm_aesenc_si128(x7, x6);
	*h8 = _mm_aesenc_si128(x8, x7);
}

void
aes_hash2_start_seed(aes_hash2_state_t * const state, const void * const seed)
{
	__m128i s;

	s = _mm_loadu_si128(seed);
	state->x1 = _mm_xor_si128(k1, s);
	state->x2 = _mm_xor_si128(k2, s);
	state->x3 = _mm_xor_si128(k3, s);
	state->x4 = _mm_xor_si128(k4, s);
	state->x5 = _mm_xor_si128(k5, s);
	state->x6 = _mm_xor_si128(k6, s);
	state->x7 = _mm_xor_si128(k7, s);
	state->x8 = _mm_xor_si128(k8, s);
	state->block_size = sizeof(state->buf);
	state->count = 0;
	state->total_len = 0;
}

void
aes_hash2_start(aes_hash2_state_t * const state)
{
	uint64_t seed[2] = { 0, 0 };

	aes_hash2_start_seed(state, seed);
}

void
aes_hash2_update(aes_hash2_state_t * const state, const void * const data,
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

	/* Do the memcpy before retrieving hash state otherwise we need
	 * to save/restore xmm registers around the memcpy call */
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

		x1 = _mm_aesenc_si128(x1, v[0]);
		x2 = _mm_aesenc_si128(x2, v[1]);
		x3 = _mm_aesenc_si128(x3, v[2]);
		x4 = _mm_aesenc_si128(x4, v[3]);
		x5 = _mm_aesenc_si128(x5, v[4]);
		x6 = _mm_aesenc_si128(x6, v[5]);
		x7 = _mm_aesenc_si128(x7, v[6]);
		x8 = _mm_aesenc_si128(x8, v[7]);

		mix1024(&x1, &x2, &x3, &x4, &x5, &x6, &x7, &x8);

		state->count = 0;
	}

	while (llen >= state->block_size) {
		v = (__m128i_u *)curr;

		x1 = _mm_aesenc_si128(x1, _mm_loadu_si128(&v[0]));
		x2 = _mm_aesenc_si128(x2, _mm_loadu_si128(&v[1]));
		x3 = _mm_aesenc_si128(x3, _mm_loadu_si128(&v[2]));
		x4 = _mm_aesenc_si128(x4, _mm_loadu_si128(&v[3]));
		x5 = _mm_aesenc_si128(x5, _mm_loadu_si128(&v[4]));
		x6 = _mm_aesenc_si128(x6, _mm_loadu_si128(&v[5]));
		x7 = _mm_aesenc_si128(x7, _mm_loadu_si128(&v[6]));
		x8 = _mm_aesenc_si128(x8, _mm_loadu_si128(&v[7]));

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
aes_hash2_end(aes_hash2_state_t * const state, aes_hash2_t * const hash)
{
	__m128i x1, x2, x3, x4, x5, x6, x7, x8;
	__m128i h[8];
	uint64_t total_len;
	uint32_t len, i;

	total_len = state->total_len;
	len = (state->count < 120) ? 120 - state->count : 248 - state->count;
	aes_hash2_update(state, padding, len);
	aes_hash2_update(state, &total_len, 8);

	x1 = state->x1;
	x2 = state->x2;
	x3 = state->x3;
	x4 = state->x4;
	x5 = state->x5;
	x6 = state->x6;
	x7 = state->x7;
	x8 = state->x8;

	for (i = 0; i < 12; i++)
		mix1024(&x1, &x2, &x3, &x4, &x5, &x6, &x7, &x8);

	h[0] = x1;
	h[1] = x2;
	h[2] = x3;
	h[3] = x4;
	h[4] = x5;
	h[5] = x6;
	h[6] = x7;
	h[7] = x8;

	memcpy(hash, h, sizeof(h));
}

void
aes_hash2_seed(const void * const data, const size_t len,
    const void * const seed, aes_hash2_t * const hash)
{
	aes_hash2_state_t aes_hash;

	aes_hash2_start_seed(&aes_hash, seed);
	aes_hash2_update(&aes_hash, data, len);
	aes_hash2_end(&aes_hash, hash);
}

void
aes_hash2(const void * const data, const size_t len, aes_hash2_t * const hash)
{
	uint64_t seed[2] = { 0, 0 };

	aes_hash2_seed(data, len, seed, hash);
}
