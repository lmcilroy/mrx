#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include "mlx_hash4.h"

static const uint64_t k0 = 0xc3a5c85c97cb3127ULL;
static const uint64_t k1 = 0xb492b66fbe98f273ULL;

static inline uint64_t
rotate64(const uint64_t x, const uint8_t s)
{
	return (x << s) | (x >> (64 - s));
}

static inline void
mix128(uint64_t * const h)
{
	unsigned long long hi;

	h[0] ^= k0;
	h[1] ^= k1;

	h[0] = _mulx_u64(h[0], h[1], &hi);
	h[1] = hi;
}

static inline void
round128(uint64_t * const h, uint64_t * const x)
{
	h[0] ^= x[0];
	h[1] ^= x[1];

	mix128(h);
}

static inline uint64_t
final64(const uint64_t h)
{
	return rotate64(h, 29) * k1;
}

void
mlx_hash4_seed(const void * const data, const size_t len, const uint64_t seed,
    mlx_hash4_t * const hash)
{
	const unsigned char *curr = data;
	uint64_t h[2];
	uint64_t x[2];
	size_t llen;

	llen = len;

	h[0] = 0;
	h[1] = 0;

	while (llen >= 16) {

		memcpy(x, curr, 16);
		round128(h, x);
		curr += 16;
		llen -= 16;
	}

	if (llen > 0) {

		x[0] = 0;
		x[1] = 0;

		memcpy(x, curr, llen);
		round128(h, x);
	}

	x[0] = seed;
	x[1] = len;
	round128(h, x);

	mix128(h);
	mix128(h);

	h[0] = final64(h[0]);
	h[1] = final64(h[1]);

	memcpy(hash, h, sizeof(h));
}

void
mlx_hash4(const void * const data, const size_t len, mlx_hash4_t * const hash)
{
	mlx_hash4_seed(data, len, 0, hash);
}
