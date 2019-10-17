#include <sys/types.h>
#include <stdint.h>

#include "aes_hash2.h"

void
aes_hash2_test(const void *key, int len, uint32_t seed, void *out)
{
	uint32_t s[4] = { seed, seed, seed, seed };

	aes_hash2_seed(key, len, s, out);
}

void
aes_hash2_test2(const void *key, int len, const void *seed, void *out)
{
	aes_hash2_seed(key, len, seed, out);
}
