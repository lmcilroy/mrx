#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include "mrx_hash32.h"

void
mrx_hash_32_test(const void *key, int len, uint32_t seed, void *out)
{
	mrx_hash_32_seed(key, len, seed, out);
}

void
mrx_hash_32_test2(const void *key, int len, const void *seed, void *out)
{
	uint32_t s;
	memcpy(&s, seed, sizeof(s));
	mrx_hash_32_seed(key, len, s, out);
}
