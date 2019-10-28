#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include "mlx_hash3.h"

void
mlx_hash3_test(const void *key, int len, uint32_t seed, void *out)
{
	mlx_hash3_seed(key, len, seed, out);
}

void
mlx_hash3_test2(const void *key, int len, const void *seed, void *out)
{
	uint64_t s;
	memcpy(&s, seed, sizeof(s));
	mlx_hash3_seed(key, len, s, out);
}
