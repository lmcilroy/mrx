#ifdef __cplusplus
extern "C" {
#endif

#include <x86intrin.h>

struct sse_hash_state {
	union {
		__m128i ints[4];
		uint8_t chars[64];
	} buf;
	uint32_t count;
	uint32_t block_size;
	uint64_t total_len;
	__m128i x1;
	__m128i x2;
	__m128i x3;
	__m128i x4;
};

typedef struct sse_hash_state sse_hash_state_t;

/* Streaming interfaces */
void sse_hash_start(sse_hash_state_t * const state);
void sse_hash_start_seed(sse_hash_state_t * const state,
    const void * const seed);
void sse_hash_update(sse_hash_state_t * const state, const void * const data,
    const size_t len);
void sse_hash_end(sse_hash_state_t * const state, void * const hash);

/* Convenience routines for single shot hashing */
void sse_hash_seed(const void * const data, const size_t len,
    const void * const seed, void * const hash);
void sse_hash(const void * const data, const size_t len, void * const hash);

#ifdef __cplusplus
}
#endif
