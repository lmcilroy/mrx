#include <sys/types.h>
#include <stdint.h>

#include "clm_hash2.h"

void
clm_hash2_test(const void *key, int len, uint32_t seed, void *out)
{
	uint32_t s[4] = { seed, seed, seed, seed };

	clm_hash2_seed(key, len, s, out);
}

void
clm_hash2_test2(const void *key, int len, const void *seed, void *out)
{
	clm_hash2_seed(key, len, seed, out);
}
