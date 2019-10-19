#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <x86intrin.h>

#include "aes_hash.h"

static const __m128i k1 = { 0xc05bbd6b47c57574, 0xf83c1e9e4a934534 };
static const __m128i k2 = { 0xa224e4507e645d91, 0xe7a9131a4b842813 };
static const __m128i k3 = { 0x58f082e3580f347e, 0xceb5795349196aff };
static const __m128i k4 = { 0xb549200b5168588f, 0xfd5c07200540ada5 };
static const __m128i k5 = { 0x526b8e90108029d5, 0x46e634784c1350f0 };
static const __m128i k6 = { 0x4e9a8c21921fc259, 0x8f5d6730b44e65c3 };
static const __m128i k7 = { 0xac0120817de73b56, 0xa5be99de5ff769dd };
static const __m128i k8 = { 0x7541811329b4609f, 0x4c4cfa866064ec93 };

void
aes_hash_start_seed(aes_hash_state_t * const state, const void * const seed)
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
aes_hash_start(aes_hash_state_t * const state)
{
	uint64_t seed[2] = { 0, 0 };

	aes_hash_start_seed(state, seed);
}

void
aes_hash_update(aes_hash_state_t * const state, const void * const data,
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
aes_hash_end(aes_hash_state_t * const state, aes_hash_t * const hash)
{
	__m128i h, x1, x2, x3, x4, x5, x6, x7, x8;
	uint64_t total_len;
	uint32_t len, i;

	total_len = state->total_len;
	len = (state->count < 120) ? 120 - state->count : 248 - state->count;
	aes_hash_update(state, padding, len);
	aes_hash_update(state, &total_len, 8);

	x1 = state->x1;
	x2 = state->x2;
	x3 = state->x3;
	x4 = state->x4;
	x5 = state->x5;
	x6 = state->x6;
	x7 = state->x7;
	x8 = state->x8;

	/* Ensure the last input has been fully mixed into each stream */
	for (i = 0; i < 4; i++) {
		x1 = _mm_aesenc_si128(x1, k1);
		x2 = _mm_aesenc_si128(x2, k2);
		x3 = _mm_aesenc_si128(x3, k3);
		x4 = _mm_aesenc_si128(x4, k4);
		x5 = _mm_aesenc_si128(x5, k5);
		x6 = _mm_aesenc_si128(x6, k6);
		x7 = _mm_aesenc_si128(x7, k7);
		x8 = _mm_aesenc_si128(x8, k8);
	}

	/* Combine the streams into one hash */
	h = _mm_setzero_si128();
	h = _mm_aesenc_si128(h, x1);
	h = _mm_aesenc_si128(h, x2);
	h = _mm_aesenc_si128(h, x3);
	h = _mm_aesenc_si128(h, x4);
	h = _mm_aesenc_si128(h, x5);
	h = _mm_aesenc_si128(h, x6);
	h = _mm_aesenc_si128(h, x7);
	h = _mm_aesenc_si128(h, x8);

	_mm_storeu_si128((__m128i_u *)hash, h);
}

void
aes_hash_seed(const void * const data, const size_t len,
    const void * const seed, aes_hash_t * const hash)
{
	aes_hash_state_t aes_hash;

	aes_hash_start_seed(&aes_hash, seed);
	aes_hash_update(&aes_hash, data, len);
	aes_hash_end(&aes_hash, hash);
}

void
aes_hash(const void * const data, const size_t len, aes_hash_t * const hash)
{
	uint64_t seed[2] = { 0, 0 };

	aes_hash_seed(data, len, seed, hash);
}
