#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <x86intrin.h>

#include "sse_hash.h"

static const __m128i k1 = { 0xca95ca95ca95ca95, 0xca95ca95ca95ca95 };

static const __m128i s1 = { 0xc05bbd6b47c57574, 0xf83c1e9e4a934534 };
static const __m128i s2 = { 0xa224e4507e645d91, 0xe7a9131a4b842813 };
static const __m128i s3 = { 0x58f082e3580f347e, 0xceb5795349196aff };
static const __m128i s4 = { 0xb549200b5168588f, 0xfd5c07200540ada5 };

static inline __m128i
mix128(const __m128i h)
{
	__m128i a, b, c;

	a = _mm_xor_si128(h, k1);
	b = _mm_mullo_epi16(a, k1);
	a = _mm_alignr_epi8(a, a, 2);
	c = _mm_mulhi_epi16(a, k1);
	return _mm_xor_si128(b, c);
}

static inline __m128i
round128(const __m128i h, const __m128i x)
{
	return mix128(_mm_xor_si128(h, x));
}

void
sse_hash_start_seed(sse_hash_state_t * const state, const void * const seed)
{
	__m128i s;

	s = _mm_loadu_si128(seed);
	state->x1 = _mm_xor_si128(s1, s);
	state->x2 = _mm_xor_si128(s2, s);
	state->x3 = _mm_xor_si128(s3, s);
	state->x4 = _mm_xor_si128(s4, s);
	state->block_size = sizeof(state->buf);
	state->count = 0;
	state->total_len = 0;
}

void
sse_hash_start(sse_hash_state_t * const state)
{
	uint64_t seed[2] = { 0, 0 };

	sse_hash_start_seed(state, &seed);
}

void
sse_hash_update(sse_hash_state_t * const state, const void * const data,
    const size_t len)
{
	__m128i *v;
	__m128i x1, x2, x3, x4;
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

	if (state->count > 0) {
		v = state->buf.ints;

		x1 = round128(x1, v[0]);
		x2 = round128(x2, v[1]);
		x3 = round128(x3, v[2]);
		x4 = round128(x4, v[3]);

		state->count = 0;
	}

	while (llen >= state->block_size) {
		v = (__m128i *)curr;

		x1 = round128(x1, _mm_loadu_si128(&v[0]));
		x2 = round128(x2, _mm_loadu_si128(&v[1]));
		x3 = round128(x3, _mm_loadu_si128(&v[2]));
		x4 = round128(x4, _mm_loadu_si128(&v[3]));

		curr += state->block_size;
		llen -= state->block_size;
	}

	state->x1 = x1;
	state->x2 = x2;
	state->x3 = x3;
	state->x4 = x4;

	if (llen > 0) {
		memcpy(state->buf.chars, curr, llen);
		state->count = llen;
	}
}

static const unsigned char padding[256] = { 0x80 };

void
sse_hash_end(sse_hash_state_t * const state, sse_hash_t * const hash)
{
	__m128i h, x1, x2, x3, x4;
	uint64_t total_len;
	uint32_t len, i;

	total_len = state->total_len;
	len = (state->count < 56) ? 56 - state->count : 120 - state->count;
	sse_hash_update(state, padding, len);
	sse_hash_update(state, &total_len, 8);

	x1 = state->x1;
	x2 = state->x2;
	x3 = state->x3;
	x4 = state->x4;

	/* Propogate dispersion over whole state */
	for (i = 0; i < 24; i++) {
		x1 = mix128(x1);
		x2 = mix128(x2);
		x3 = mix128(x3);
		x4 = mix128(x4);
	}

	/* Combine the streams into one hash */
	h = _mm_setzero_si128();
	h = round128(h, x1);
	h = round128(h, x2);
	h = round128(h, x3);
	h = round128(h, x4);

	_mm_storeu_si128((__m128i *)hash, h);
}

void
sse_hash_seed(const void * const data, const size_t len,
    const void * const seed, sse_hash_t * const hash)
{
	sse_hash_state_t sse_hash;

	sse_hash_start_seed(&sse_hash, seed);
	sse_hash_update(&sse_hash, data, len);
	sse_hash_end(&sse_hash, hash);
}

void
sse_hash(const void * const data, const size_t len, sse_hash_t * const hash)
{
	uint64_t seed[2] = { 0, 0 };

	sse_hash_seed(data, len, &seed, hash);
}
