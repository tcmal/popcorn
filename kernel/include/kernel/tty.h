#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

// Standard interface for putting text on the screen
// The actual TTY implementation is architecture-dependent.

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// Not all TTYs necessarily support all these colours
enum ttycolor {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
typedef enum ttycolor ttycolor_t;

// Convert a foreground & background to a format specifier.
uint8_t tty_format(ttycolor_t, ttycolor_t);
void tty_init();
bool tty_supportsFormat(uint8_t);
void tty_setFormat(uint8_t);
void tty_charAt(unsigned char, uint8_t, size_t, size_t);
void tty_putChar(char);
void tty_nextRow();
void tty_writeN(const char*, size_t);
void tty_writeString(const char*);

// TODO: Move this
int pow(int sub, int exp);

#endif
