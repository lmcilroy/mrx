#ifdef __cplusplus
extern "C" {
#endif

#include <x86intrin.h>

struct clm_hash2_state {
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

struct clm_hash2 {
	__m128i h[8];
};

typedef struct clm_hash2_state clm_hash2_state_t;
typedef struct clm_hash2 clm_hash2_t;

/* Streaming interfaces */
void clm_hash2_start(clm_hash2_state_t * const state);
void clm_hash2_start_seed(clm_hash2_state_t * const state,
    const void * const seed);
void clm_hash2_update(clm_hash2_state_t * const state,
    const void * const data, const size_t len);
void clm_hash2_end(clm_hash2_state_t * const state, clm_hash2_t * const hash);

/* Convenience routines for single shot hashing */
void clm_hash2_seed(const void * const data, const size_t len,
    const void * const seed, clm_hash2_t * const hash);
void clm_hash2(const void * const data, const size_t len,
    clm_hash2_t * const hash);

#ifdef __cplusplus
}
#endif
