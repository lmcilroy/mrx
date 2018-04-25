#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include "mrx_hash32.h"
#include "mem.h"

static const uint32_t c0 = 0xcc9e2d51;
static const uint32_t c1 = 0x1b873593;
static const uint32_t c2 = 0xe6546b69;
static const uint32_t c3 = 0x9e3779b1;
static const uint32_t c4 = 0x1e35a7bd;
static const uint32_t c5 = 0xc2b2ae35;

static inline uint32_t
rotate32(const uint32_t x, const uint8_t s)
{
	return (x << s) | (x >> (32 - s));
}

static inline uint32_t
round32(const uint32_t h, const uint32_t x)
{
	return rotate32(h ^ (x * c0), 15) * c1;
}

static inline uint32_t
final32(const uint32_t h)
{
	return rotate32(h, 15) * c4;
}

void
mrx_hash_32_start_seed(mrx_state_32_t * const state, const uint32_t seed)
{
	state->x1 = c2 ^ seed;
	state->x2 = c3 ^ seed;
	state->x3 = c4 ^ seed;
	state->x4 = c5 ^ seed;
	state->block_size = sizeof(state->buf);
	state->total_len = 0;
	state->count = 0;
}

void
mrx_hash_32_start(mrx_state_32_t * const state)
{
	mrx_hash_32_start_seed(state, 0);
}

void
mrx_hash_32_update(mrx_state_32_t * const state, const void * const data,
    const size_t len)
{
	const unsigned char *curr = data;
	uint32_t *v;
	size_t llen;
	uint32_t left, x1, x2, x3, x4;

	if (data == NULL || len == 0)
		return;

	state->total_len += len;

	if (state->count + len < state->block_size) {
		memcpy(state->buf.chars + state->count, data, len);
		state->count += len;
		return;
	}

	llen = len;
	x1 = state->x1;
	x2 = state->x2;
	x3 = state->x3;
	x4 = state->x4;

	if (state->count > 0) {
		left = state->block_size - state->count;
		memcpy(state->buf.chars + state->count, data, left);
		llen -= left;
		curr += left;

		v = state->buf.ints;
		x1 = round32(x1, v[0]);
		x2 = round32(x2, v[1]);
		x3 = round32(x3, v[2]);
		x4 = round32(x4, v[3]);

		state->count = 0;
	}

	while (llen >= state->block_size) {
		v = (uint32_t *)curr;
		x1 = round32(x1, readmem32(&v[0]));
		x2 = round32(x2, readmem32(&v[1]));
		x3 = round32(x3, readmem32(&v[2]));
		x4 = round32(x4, readmem32(&v[3]));
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
mrx_hash_32_end(mrx_state_32_t * const state, void * const hash)
{
	uint32_t h, x1, x2, x3, x4;
	uint32_t total_len;
	uint32_t len;

	total_len = state->total_len;
	len = (state->count < 12) ? 12 - state->count : 28 - state->count;
	mrx_hash_32_update(state, padding, len);
	mrx_hash_32_update(state, &total_len, 4);

	x1 = final32(state->x1);
	x2 = final32(state->x2);
	x3 = final32(state->x3);
	x4 = final32(state->x4);

	h = round32(0, x1);
	h = round32(h, x2);
	h = round32(h, x3);
	h = round32(h, x4);
	h = final32(h);

	memcpy(hash, &h, sizeof(h));
}

void
mrx_hash_32_seed(const void * const data, const size_t len,
    const uint32_t seed, void * const hash)
{
	mrx_state_32_t state;

	mrx_hash_32_start_seed(&state, seed);
	mrx_hash_32_update(&state, data, len);
	mrx_hash_32_end(&state, hash);
}

void
mrx_hash_32(const void * const data, const size_t len,
    void * const hash)
{
	mrx_hash_32_seed(data, len, 0, hash);
}
