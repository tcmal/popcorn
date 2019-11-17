#include <string.h>

// 123456

// 654321

// Convert 
void int_to_str(int num, char* str, size_t buflen) {
	str[0] = '0';
	int largestIndex = 0;
	for (int exp = 0; exp < buflen - 1; exp++) {
		int place = pow(10, exp);
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