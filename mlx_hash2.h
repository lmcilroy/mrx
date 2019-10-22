#ifdef __cplusplus
extern "C" {
#endif

#include <x86intrin.h>

struct mlx_hash2_state {
	union {
		uint64_t ints[4];
		uint8_t chars[32];
	} buf;
	uint32_t count;
	uint32_t block_size;
	uint64_t total_len;
	uint64_t x1;
	uint64_t x2;
	uint64_t x3;
	uint64_t x4;
};

struct mlx_hash2 {
	uint64_t h[4];
};

typedef struct mlx_hash2_state mlx_hash2_state_t;
typedef struct mlx_hash2 mlx_hash2_t;

/* 256-bit streaming interfaces */
void mlx_hash2_start(mlx_hash2_state_t * const state);
void mlx_hash2_start_seed(mlx_hash2_state_t * const state,
    const uint64_t seed);
void mlx_hash2_update(mlx_hash2_state_t * const state,
    const void * const data, const size_t len);
void mlx_hash2_end(mlx_hash2_state_t * const state, mlx_hash2_t * const hash);

/* 256-bit convenience routines for single shot hashing */
void mlx_hash2_seed(const void * const data, const size_t len,
    const uint64_t seed, mlx_hash2_t * const hash);
void mlx_hash2(const void * const data, const size_t len,
    mlx_hash2_t * const hash);

#ifdef __cplusplus
}
#endif
