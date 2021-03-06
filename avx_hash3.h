#ifdef __cplusplus
extern "C" {
#endif

#include <x86intrin.h>

struct avx_hash3_state {
	union {
		__m256i ints[8];
		uint8_t chars[256];
	} buf;
	uint32_t count;
	uint32_t block_size;
	uint64_t total_len;
	__m256i x1;
	__m256i x2;
	__m256i x3;
	__m256i x4;
	__m256i x5;
	__m256i x6;
	__m256i x7;
	__m256i x8;
};

struct avx_hash3 {
	uint64_t h[4];
};

typedef struct avx_hash3_state avx_hash3_state_t;
typedef struct avx_hash3 avx_hash3_t;

/* Streaming interfaces */
void avx_hash3_start(avx_hash3_state_t * const state);
void avx_hash3_start_seed(avx_hash3_state_t * const state,
    const void * const seed);
void avx_hash3_update(avx_hash3_state_t * const state, const void * const data,
    const size_t len);
void avx_hash3_end(avx_hash3_state_t * const state, avx_hash3_t * const hash);

/* Convenience routines for single shot hashing */
void avx_hash3_seed(const void * const data, const size_t len,
    const void * const seed, avx_hash3_t * const hash);
void avx_hash3(const void * const data, const size_t len,
    avx_hash3_t * const hash);

#ifdef __cplusplus
}
#endif
