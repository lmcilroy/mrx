#ifdef __cplusplus
extern "C" {
#endif

/* Interface for SMHasher (Appleby) */

void sse_hash3_test(const void *key, int len, uint32_t seed, void *out);

/* Interface for SMHasher (Orton) */

void sse_hash3_test2(const void *key, int len, const void *seed, void *out);

#ifdef __cplusplus
}
#endif
