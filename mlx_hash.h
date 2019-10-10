#ifdef __cplusplus
extern "C" {
#endif

struct mlx_hash_state {
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
};

typedef struct mlx_hash_state mlx_hash_state_t;
typedef uint64_t mlx_hash_t;

/* 64-bit streaming interfaces */
void mlx_hash_start(mlx_hash_state_t * const state);
void mlx_hash_start_seed(mlx_hash_state_t * const state, const uint64_t seed);
void mlx_hash_update(mlx_hash_state_t * const state, const void * const data,
    const size_t len);
void mlx_hash_end(mlx_hash_state_t * const state, mlx_hash_t * const hash);

/* 64-bit convenience routines for single shot hashing */
void mlx_hash_seed(const void * const data, const size_t len,
    const uint64_t seed, mlx_hash_t * const hash);
void mlx_hash(const void * const data, const size_t len,
    mlx_hash_t * const hash);

#ifdef __cplusplus
}
#endif
