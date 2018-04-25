#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include "mrx_hash64.h"

void
mrx_hash_64_test(const void *key, int len, uint32_t seed, void *out)
{
	mrx_hash_64_seed(key, len, seed, out);
}

void
mrx_hash_64_test2(const void *key, int len, const void *seed, void *out)
{
	uint64_t s;
	memcpy(&s, seed, sizeof(s));
	mrx_hash_64_seed(key, len, s, out);
}
