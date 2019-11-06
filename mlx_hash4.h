#ifdef __cplusplus
extern "C" {
#endif

#include <x86intrin.h>

struct mlx_hash4 {
	uint64_t h[2];
};

typedef struct mlx_hash4 mlx_hash4_t;

void mlx_hash4_seed(const void * const data, const size_t len,
    const uint64_t seed, mlx_hash4_t * const hash);
void mlx_hash4(const void * const data, const size_t len,
    mlx_hash4_t * const hash);

#ifdef __cplusplus
}
#endif
