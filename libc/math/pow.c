#include <math.h>
#include <limits.h>

int pow(int sub, int exp) {
	int acc = 1;
	for (int i = 0; i < exp; i++) {
		// overflow
		if (sub > INT_MAX / acc) {
			return -1;
		}
		acc = acc * sub;
	}
	return acc;
}