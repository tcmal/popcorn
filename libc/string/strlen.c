#include <string.h>

// Find length of null-terminated string
size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
