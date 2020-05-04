#pragma once

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);

#if defined(__is_libk)

__attribute__((__noreturn__))
void kpanic(const char* restrict fmt, ...);
#endif

#ifdef __cplusplus
}
#endif
