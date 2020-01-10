#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

int printf(const char* restrict formatStr, ...) {
	// Initialise dynamic argument getting
	va_list vaparams;
	va_start(vaparams, formatStr);

	int written = 0;

	// Read until done with string
	while (*formatStr != '\0') {

		size_t maxrem = INT_MAX - written;

		// Writes normal/escaped characters
		if (formatStr[0] != '%' || formatStr[1] == '%') {
			// This means formatStr starts with %%, so print from the normal %
			if (formatStr[0] == '%')
				formatStr++;

			// Print up until the next %

			// Get amount
			size_t amount = 1;
			while (formatStr[amount] && formatStr[amount] != '%')
				amount++;

			// Deal with overflows
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}

			// Try to print
			if (!print(formatStr, amount))
				return -1;

			// Record written and move on
			formatStr += amount;
			written += amount;
			continue;
		}

		// Special string
		const char* formatStr_begun_at = formatStr++;

		if (*formatStr == 'c') {
			// Single character, just print it
			formatStr++;
			char c = (char) va_arg(vaparams, int /* char promotes to int */);

			// Deals with overflow
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}

			// Try to print
			if (!print(&c, sizeof(c)))
				return -1;

			written++;
		} else if (*formatStr == 's') {
			// Put string in

			formatStr++;
			const char* str = va_arg(vaparams, const char*);
			size_t len = strlen(str);

			// Deals with overflow
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}

			// Try to print
			if (!print(str, len))
				return -1;

			written += len;
		} else if (*formatStr == 'i' || *formatStr == 'u') {
			char str[11];
			size_t len;

			// Convert number to string
			if (*formatStr == 'i') {
				int x = va_arg(vaparams, int);
				len = int_to_str(x, str, 11);
			} else {
				unsigned int x = va_arg(vaparams, unsigned int);
				len = int_to_str_unsigned(x, str, 11);
			}

			formatStr++;

			// Deals with overflow
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}

			// Try to print
			if (!print(str, len)) {
				return -1;
			}

			written += len;
		} else {
			// Otherwise, print that format specifier raw
			formatStr = formatStr_begun_at;
			size_t len = strlen(formatStr);

			// Overflow
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}

			// Try to print
			if (!print(formatStr, len))
				return -1;

			written += len;
			formatStr += len;
		}
	}

	va_end(vaparams);
	return written;
}
