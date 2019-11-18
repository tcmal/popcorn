#include <string.h>
#include <stdint.h>

void int_to_str(int num, char* str, size_t buflen) {
	if (num < 0) {
		num = -num;

		str[0] = '-';
		str = &str[1];
		buflen--;
	} else {
		str[0] = '0';
	}

	int largestIndex = 0;
	for (int exp = 0; exp < buflen - 1; exp++) {
		int place = pow(10, exp);
		if (place == -1) {
			break;
		}
		if ((num / place) >= 1) {
			int col = (num / place) % 10;
			str[exp] = col + 48;
			largestIndex = exp;
		} else {
			break;
		}
	}

	// reverse the parts we wrote
	char temp;
	for (int i = 0; i <= (largestIndex / 2); i++) {
		temp = str[largestIndex - i];
		str[largestIndex - i] = str[i];
		str[i] = temp;
	}

	str[largestIndex + 1] = 0;
}

void int_to_str_unsigned(unsigned int num, char* str, size_t buflen) {
	str[0] = '0';

	int largestIndex = 0;
	for (int exp = 0; exp < buflen - 1; exp++) {
		int place = pow(10, exp);
		if (place == -1) {
			break;
		}

		if ((num / place) >= 1) {
			int col = (num / place) % 10;
			str[exp] = col + 48;
			largestIndex = exp;
		} else {
			break;
		}
	}

	// reverse the parts we wrote
	char temp;
	for (int i = 0; i <= (largestIndex / 2); i++) {
		temp = str[largestIndex - i];
		str[largestIndex - i] = str[i];
		str[i] = temp;
	}

	str[largestIndex + 1] = 0;
}