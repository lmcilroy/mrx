#ifdef __cplusplus
extern "C" {
#endif

#include <x86intrin.h>

struct sse_hash3_state {
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

struct sse_hash3 {
	uint64_t h[2];
};

typedef struct sse_hash3_state sse_hash3_state_t;
typedef struct sse_hash3 sse_hash3_t;

/* Streaming interfaces */
void sse_hash3_start(sse_hash3_state_t * const state);
void sse_hash3_start_seed(sse_hash3_state_t * const state,
    const void * const seed);
void sse_hash3_update(sse_hash3_state_t * const state,
    const void * const data, const size_t len);
void sse_hash3_end(sse_hash3_state_t * const state,
    sse_hash3_t * const hash);

/* Convenience routines for single shot hashing */
void sse_hash3_seed(const void * const data, const size_t len,
    const void * const seed, sse_hash3_t * const hash);
void sse_hash3(const void * const data, const size_t len,
    sse_hash3_t * const hash);

#ifdef __cplusplus
}
#endif
