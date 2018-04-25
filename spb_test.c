#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include "spb_hash.h"

void
spb_hash_test(const void *key, int len, uint32_t seed, void *out)
{
	spb_hash_seed(key, len, seed, out);
}

void
spb_hash_test2(const void *key, int len, const void *seed, void *out)
{
	uint64_t s;
	memcpy(&s, seed, sizeof(s));
	spb_hash_seed(key, len, s, out);
}
