#ifdef __cplusplus
extern "C" {
#endif

#include <x86intrin.h>

struct aes_hash_state {
	union {
		__m128i ints[8];
		uint8_t chars[128];
	} buf;
	uint32_t count;
	uint32_t block_size;
	uint64_t total_len;
	__m128i x1;
	__m128i x2;
	__m128i x3;
	__m128i x4;
	__m128i x5;
	__m128i x6;
	__m128i x7;
	__m128i x8;
};

typedef struct aes_hash_state aes_hash_state_t;

/* Streaming interfaces */
void aes_hash_start(aes_hash_state_t * const state);
void aes_hash_start_seed(aes_hash_state_t * const state, const void * const seed);
void aes_hash_update(aes_hash_state_t * const state, const void * const data,
    const size_t len);
void aes_hash_end(aes_hash_state_t * const state, void * const hash);

/* Convenience routines for single shot hashing */
void aes_hash_seed(const void * const data, const size_t len,
    const void * const seed, void * const hash);
void aes_hash(const void * const data, const size_t len, void * const hash);

#ifdef __cplusplus
}
#endif
