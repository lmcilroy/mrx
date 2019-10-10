#ifdef __cplusplus
extern "C" {
#endif

struct mrx_hash_64_state {
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

typedef struct mrx_hash_64_state mrx_hash_64_state_t;
typedef uint64_t mrx_hash_64_t;

/* 64-bit streaming interfaces */
void mrx_hash_64_start(mrx_hash_64_state_t * const state);
void mrx_hash_64_start_seed(mrx_hash_64_state_t * const state,
    const uint64_t seed);
void mrx_hash_64_update(mrx_hash_64_state_t * const state,
    const void * const data, const size_t len);
void mrx_hash_64_end(mrx_hash_64_state_t * const state,
    mrx_hash_64_t * const hash);

/* 64-bit convenience routines for single shot hashing */
void mrx_hash_64_seed(const void * const data, const size_t len,
    const uint64_t seed, mrx_hash_64_t * const hash);
void mrx_hash_64(const void * const data, const size_t len,
    mrx_hash_64_t * const hash);

#ifdef __cplusplus
}
#endif
