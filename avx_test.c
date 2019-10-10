#include <sys/types.h>
#include <stdint.h>

#include "avx_hash.h"

void
avx_hash_test(const void *key, int len, uint32_t seed, void *out)
{
	uint32_t s[8] = { seed, seed, seed, seed, seed, seed, seed, seed };

	avx_hash_seed(key, len, s, out);
}

void
avx_hash_test2(const void *key, int len, const void *seed, void *out)
{
	avx_hash_seed(key, len, seed, out);
}
