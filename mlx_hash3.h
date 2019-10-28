#ifdef __cplusplus
extern "C" {
#endif

#include <x86intrin.h>

struct mlx_hash3_state {
	union {
		uint64_t ints[8];
		uint8_t chars[64];
	} buf;
	uint32_t count;
	uint32_t block_size;
	uint64_t total_len;
	uint64_t x1;
	uint64_t x2;
	uint64_t x3;
	uint64_t x4;
	uint64_t x5;
	uint64_t x6;
	uint64_t x7;
	uint64_t x8;
};

struct mlx_hash3 {
	uint64_t h[2];
};

typedef struct mlx_hash3_state mlx_hash3_state_t;
typedef struct mlx_hash3 mlx_hash3_t;

/* 128-bit streaming interfaces */
void mlx_hash3_start(mlx_hash3_state_t * const state);
void mlx_hash3_start_seed(mlx_hash3_state_t * const state,
    const uint64_t seed);
void mlx_hash3_update(mlx_hash3_state_t * const state,
    const void * const data, const size_t len);
void mlx_hash3_end(mlx_hash3_state_t * const state, mlx_hash3_t * const hash);

/* 128-bit convenience routines for single shot hashing */
void mlx_hash3_seed(const void * const data, const size_t len,
    const uint64_t seed, mlx_hash3_t * const hash);
void mlx_hash3(const void * const data, const size_t len,
    mlx_hash3_t * const hash);

#ifdef __cplusplus
}
#endif
