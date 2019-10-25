#include <sys/types.h>
#include <stdint.h>

#include "sse_hash3.h"

void
sse_hash3_test(const void *key, int len, uint32_t seed, void *out)
{
	uint32_t s[4] = { seed, seed, seed, seed };

	sse_hash3_seed(key, len, s, out);
}

void
sse_hash3_test2(const void *key, int len, const void *seed, void *out)
{
	sse_hash3_seed(key, len, seed, out);
}
