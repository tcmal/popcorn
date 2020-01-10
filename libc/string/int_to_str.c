#include <stdbool.h>
#include <string.h>
#include <stdint.h>

size_t int_to_str(int num, char* str, size_t buflen) {
	int len = 0;

	if (num < 0) {
		num = -num;
		str[0] = '-';
		buflen--;
		len++;
	} else if (num == 0){
		str[0] = '0';
		str[1] = 0;
		return 2;
	}

	for (int exp = buflen - 1; exp >= 0; exp--) {
		int place = pow(10, exp);
		if (place == -1) {
			continue;
		}

		if ((num / place) >= 1) {
			int col = (num / place) % 10;
			str[len] = col + 48;
			len++;
		}
	}

	str[len++] = 0;

	return len;
}

size_t int_to_str_unsigned(unsigned int num, char* str, size_t buflen) {
	int len = 0;

	if (num == 0) {
		str[0] = '0';
		str[1] = 0;
		return 2;
	}

	for (int exp = buflen - 1; exp >= 0; exp--) {
		int place = pow(10, exp);
		if (place == -1) {
			continue;
		}

		if ((num / place) >= 1) {
			int col = (num / place) % 10;
			str[len] = col + 48;
			len++;
		}
	}

	str[len++] = 0;
	return len;
}