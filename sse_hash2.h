#ifdef __cplusplus
extern "C" {
#endif

#include <x86intrin.h>

struct sse_hash2_state {
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

struct sse_hash2 {
	uint64_t h[2];
};

typedef struct sse_hash2_state sse_hash2_state_t;
typedef struct sse_hash2 sse_hash2_t;

/* Streaming interfaces */
void sse_hash2_start(sse_hash2_state_t * const state);
void sse_hash2_start_seed(sse_hash2_state_t * const state,
    const void * const seed);
void sse_hash2_update(sse_hash2_state_t * const state, const void * const data,
    const size_t len);
void sse_hash2_end(sse_hash2_state_t * const state, sse_hash2_t * const hash);

/* Convenience routines for single shot hashing */
void sse_hash2_seed(const void * const data, const size_t len,
    const void * const seed, sse_hash2_t * const hash);
void sse_hash2(const void * const data, const size_t len,
    sse_hash2_t * const hash);

#ifdef __cplusplus
}
#endif
