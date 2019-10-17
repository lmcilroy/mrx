#ifdef __cplusplus
extern "C" {
#endif

/* Interface for SMHasher (Appleby) */

void aes_hash2_test(const void *key, int len, uint32_t seed, void *out);

/* Interface for SMHasher (Orton) */

void aes_hash2_test2(const void *key, int len, const void *seed, void *out);

#ifdef __cplusplus
}
#endif
