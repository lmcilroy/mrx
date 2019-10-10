#ifdef __cplusplus
extern "C" {
#endif

struct mrx_hash_32_state {
	union {
		uint32_t ints[4];
		uint8_t chars[16];
	} buf;
	uint32_t count;
	uint32_t block_size;
	uint32_t total_len;
	uint32_t x1;
	uint32_t x2;
	uint32_t x3;
	uint32_t x4;
};

typedef struct mrx_hash_32_state mrx_hash_32_state_t;
typedef uint32_t mrx_hash_32_t;

/* 32-bit streaming interfaces */
void mrx_hash_32_start(mrx_hash_32_state_t * const state);
void mrx_hash_32_start_seed(mrx_hash_32_state_t * const state,
    const uint32_t seed);
void mrx_hash_32_update(mrx_hash_32_state_t * const state,
    const void * const data, const size_t len);
void mrx_hash_32_end(mrx_hash_32_state_t * const state,
    mrx_hash_32_t * const hash);

/* 32-bit convenience routines for single shot hashing */
void mrx_hash_32_seed(const void * const data, const size_t len,
    const uint32_t seed, mrx_hash_32_t * const hash);
void mrx_hash_32(const void * const data, const size_t len,
    mrx_hash_32_t * const hash);

#ifdef __cplusplus
}
#endif
