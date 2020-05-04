#include <stdio.h>
#include <stdlib.h>

#if defined(__is_libk)
__attribute__((__noreturn__))
void kpanic(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	// TODO: Add proper kernel panic.
	printf("UwU, we made a fucky wucky: ");

	vaprintf(fmt, args);

	while (1) { }
	__builtin_unreachable();
}

#endif