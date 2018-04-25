#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include "mlx_hash.h"
#include "mem.h"

static const uint64_t k0 = 0xc3a5c85c97cb3127ULL;
static const uint64_t k1 = 0xb492b66fbe98f273ULL;
static const uint64_t k2 = 0x9ae16a3b2f90404fULL;
static const uint64_t k3 = 0xc949d7c7509e6557ULL;
static const uint64_t k4 = 0xff51afd7ed558ccdULL;
static const uint64_t k5 = 0xc4ceb9fe1a85ec53ULL;

static inline uint64_t
rotate64(const uint64_t x, const uint8_t s)
{
	return (x << s) | (x >> (64 - s));
}

static inline uint64_t
round64(const uint64_t h, const uint64_t x0, const uint64_t x1)
{
	__uint128_t r;

	r = (__uint128_t)(h ^ x0 ^ k0) * (__uint128_t)(x1 ^ k1);
	return (uint64_t)(r) ^ (uint64_t)(r >> 64);
}

static inline uint64_t
final64(const uint64_t h)
{
	return rotate64(h, 29) * k1;
}

void
mlx_hash_start_seed(mlx_state_t * const state, const uint64_t seed)
{
	state->x1 = k2 ^ seed;
	state->x2 = k3 ^ seed;
	state->x3 = k4 ^ seed;
	state->x4 = k5 ^ seed;
	state->block_size = sizeof(state->buf);
	state->count = 0;
	state->total_len = 0;
}

void
mlx_hash_start(mlx_state_t * const state)
{
	mlx_hash_start_seed(state, 0);
}

void
mlx_hash_update(mlx_state_t * const state, const void * const data,
    const size_t len)
{
	const unsigned char *curr = data;
	uint64_t *v;
	uint64_t x1, x2, x3, x4;
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
		x1 = round64(x1, v[0], v[1]);
		x2 = round64(x2, v[2], v[3]);
		x3 = round64(x3, v[4], v[5]);
		x4 = round64(x4, v[6], v[7]);

		state->count = 0;
	}

	while (llen >= state->block_size) {
		v = (uint64_t *)curr;
		x1 = round64(x1, readmem64(&v[0]), readmem64(&v[1]));
		x2 = round64(x2, readmem64(&v[2]), readmem64(&v[3]));
		x3 = round64(x3, readmem64(&v[4]), readmem64(&v[5]));
		x4 = round64(x4, readmem64(&v[6]), readmem64(&v[7]));
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
mlx_hash_end(mlx_state_t * const state, void * const hash)
{
	uint64_t h, x1, x2, x3, x4;
	uint64_t total_len;
	uint32_t len;

	total_len = state->total_len;
	len = (state->count < 56) ? 56 - state->count : 120 - state->count;
	mlx_hash_update(state, padding, len);
	mlx_hash_update(state, &total_len, 8);

	x1 = final64(state->x1);
	x2 = final64(state->x2);
	x3 = final64(state->x3);
	x4 = final64(state->x4);

	h = round64(0, x1, x2);
	h = round64(h, x3, x4);
	h = final64(h);

	memcpy(hash, &h, sizeof(h));
}

void
mlx_hash_seed(const void * const data, const size_t len,
    const uint64_t seed, void * const hash)
{
	mlx_state_t state;

	mlx_hash_start_seed(&state, seed);
	mlx_hash_update(&state, data, len);
	mlx_hash_end(&state, hash);
}

void
mlx_hash(const void * const data, const size_t len, void * const hash)
{
	mlx_hash_seed(data, len, 0, hash);
}
