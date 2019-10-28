#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include "mlx_hash3.h"
#include "mem.h"

static const uint64_t k0 = 0xc3a5c85c97cb3127ULL;
static const uint64_t k1 = 0xb492b66fbe98f273ULL;
static const uint64_t k2 = 0x9ae16a3b2f90404fULL;
static const uint64_t k3 = 0xc949d7c7509e6557ULL;
static const uint64_t k4 = 0xff51afd7ed558ccdULL;
static const uint64_t k5 = 0xc4ceb9fe1a85ec53ULL;
static const uint64_t k6 = 0x58f082e3580f347eULL;
static const uint64_t k7 = 0xceb5795349196affULL;
static const uint64_t k8 = 0xb549200b5168588fULL;
static const uint64_t k9 = 0xfd5c07200540ada5ULL;


static inline uint64_t
rotate64(const uint64_t x, const uint8_t s)
{
        return (x << s) | (x >> (64 - s));
}

static inline uint64_t
final64(const uint64_t h)
{
        return rotate64(h, 29) * k1;
}

static inline void
mix128(uint64_t * const h1, uint64_t * const h2)
{
	unsigned long long hi;

	*h1 = _mulx_u64(*h1 ^ k0, *h2 ^ k1, &hi);
	*h2 = hi;
}

static inline void
round128(uint64_t * const h1, uint64_t * const h2, const uint64_t x1,
    const uint64_t x2)
{
	*h1 ^= x1;
	*h2 ^= x2;
	mix128(h1, h2);
}

void
mlx_hash3_start_seed(mlx_hash3_state_t * const state, const uint64_t seed)
{
	state->x1 = k2 ^ seed;
	state->x2 = k3 ^ seed;
	state->x3 = k4 ^ seed;
	state->x4 = k5 ^ seed;
	state->x5 = k6 ^ seed;
	state->x6 = k7 ^ seed;
	state->x7 = k8 ^ seed;
	state->x8 = k9 ^ seed;
	state->block_size = sizeof(state->buf);
	state->count = 0;
	state->total_len = 0;
}

void
mlx_hash3_start(mlx_hash3_state_t * const state)
{
	mlx_hash3_start_seed(state, 0);
}

void
mlx_hash3_update(mlx_hash3_state_t * const state, const void * const data,
    const size_t len)
{
	const unsigned char *curr = data;
	uint64_t *v;
	uint64_t x1, x2, x3, x4, x5, x6, x7, x8;
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
	x5 = state->x5;
	x6 = state->x6;
	x7 = state->x7;
	x8 = state->x8;

	if (state->count > 0) {
		left = state->block_size - state->count;
		memcpy(state->buf.chars + state->count, data, left);
		llen -= left;
		curr += left;

		v = state->buf.ints;
		round128(&x1, &x2, v[0], v[1]);
		round128(&x3, &x4, v[2], v[3]);
		round128(&x5, &x6, v[4], v[5]);
		round128(&x7, &x8, v[6], v[7]);

		state->count = 0;
	}

	while (llen >= state->block_size) {

		round128(&x1, &x2, readmem64(curr),    readmem64(curr+8));
		round128(&x3, &x4, readmem64(curr+16), readmem64(curr+24));
		round128(&x5, &x6, readmem64(curr+32), readmem64(curr+40));
		round128(&x7, &x8, readmem64(curr+48), readmem64(curr+56));

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
mlx_hash3_end(mlx_hash3_state_t * const state, mlx_hash3_t * const hash)
{
	uint64_t x1, x2, x3, x4, x5, x6, x7, x8;
	uint64_t h[2];
	uint64_t total_len;
	uint32_t len;

	total_len = state->total_len;
	len = (state->count < 56) ? 56 - state->count : 120 - state->count;
	mlx_hash3_update(state, padding, len);
	mlx_hash3_update(state, &total_len, 8);

	x1 = state->x1;
	x2 = state->x2;
	x3 = state->x3;
	x4 = state->x4;
	x5 = state->x5;
	x6 = state->x6;
	x7 = state->x7;
	x8 = state->x8;

	mix128(&x1, &x2);
	mix128(&x3, &x4);
	mix128(&x5, &x6);
	mix128(&x7, &x8);

	round128(&x1, &x2, x3, x4);
	round128(&x1, &x2, x5, x6);
	round128(&x1, &x2, x7, x8);

	h[0] = final64(x1);
	h[1] = final64(x2);

	memcpy(hash, &h, sizeof(h));
}

void
mlx_hash3_seed(const void * const data, const size_t len,
    const uint64_t seed, mlx_hash3_t * const hash)
{
	mlx_hash3_state_t state;

	mlx_hash3_start_seed(&state, seed);
	mlx_hash3_update(&state, data, len);
	mlx_hash3_end(&state, hash);
}

void
mlx_hash3(const void * const data, const size_t len, mlx_hash3_t * const hash)
{
	mlx_hash3_seed(data, len, 0, hash);
}
