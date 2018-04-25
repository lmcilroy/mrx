#ifdef __cplusplus
extern "C" {
#endif

/* Interfaces for SMHasher (Appleby) */

void spb_hash_test(const void *key, int len, uint32_t seed, void *out);

/* Interfaces for SMHasher (Orton) */

void spb_hash_test2(const void *key, int len, const void *seed, void *out);

#ifdef __cplusplus
}
#endif
