#ifdef __cplusplus
extern "C" {
#endif

struct mlx_state {
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

typedef struct mlx_state mlx_state_t;

/* 64-bit streaming interfaces */
void mlx_hash_start(mlx_state_t * const state);
void mlx_hash_start_seed(mlx_state_t * const state, const uint64_t seed);
void mlx_hash_update(mlx_state_t * const state, const void * const data,
    const size_t len);
void mlx_hash_end(mlx_state_t * const state, void * const hash);

/* 64-bit convenience routines for single shot hashing */
void mlx_hash_seed(const void * const data, const size_t len,
    const uint64_t seed, void * const hash);
void mlx_hash(const void * const data, const size_t len,
    void * const hash);

#ifdef __cplusplus
}
#endif
