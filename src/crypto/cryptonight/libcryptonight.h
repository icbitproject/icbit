#ifndef _LIBCRYPTONIGHT_H__
#define _LIBCRYPTONIGHT_H__
#ifdef __cplusplus
extern "C" {
#endif

void cryptonight_hash(void* output, const void* input, size_t len);

#ifdef __cplusplus
};
#endif
#endif //_LIBCRYPTONIGHT_H__
