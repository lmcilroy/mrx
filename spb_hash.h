#ifdef __cplusplus
extern "C" {
#endif

struct spb_hash_state {
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

typedef struct spb_hash_state spb_hash_state_t;
typedef uint64_t spb_hash_t;

/* 64-bit streaming interfaces */
void spb_hash_start(spb_hash_state_t * const state);
void spb_hash_start_seed(spb_hash_state_t * const state, const uint64_t seed);
void spb_hash_update(spb_hash_state_t * const state, const void * const data,
    const size_t len);
void spb_hash_end(spb_hash_state_t * const state, spb_hash_t * const hash);

/* 64-bit convenience routines for single shot hashing */
void spb_hash_seed(const void * const data, const size_t len,
    const uint64_t seed, spb_hash_t * const hash);
void spb_hash(const void * const data, const size_t len,
    spb_hash_t * const hash);

#ifdef __cplusplus
}
#endif
