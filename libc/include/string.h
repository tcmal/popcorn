#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
size_t strlen(const char*);
size_t int_to_str(int num, char* str, size_t buflen);
size_t int_to_str_unsigned(unsigned int num, char* str, size_t buflen);

#ifdef __cplusplus
}
#endif

#endif
