#ifdef __cplusplus
extern "C" {
#endif

/* Interface for SMHasher (Appleby) */

void avx_hash4_test(const void *key, int len, uint32_t seed, void *out);

/* Interface for SMHasher (Orton) */

void avx_hash4_test2(const void *key, int len, const void *seed, void *out);

#ifdef __cplusplus
}
#endif
