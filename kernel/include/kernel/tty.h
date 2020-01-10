#pragma once

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

// Called at start to initialise the TTY.
void tty_init();

// Returns if the given byte can be used as a format for this tty.
bool tty_supportsFormat(uint8_t);

// Set the current format of the TTY to this
void tty_setFormat(uint8_t);

// Sets the char at the given x,y to the set character/format.
void tty_charAt(char, uint8_t, size_t, size_t);

// Add a character to the TTY, wrapping if necessary.
void tty_putChar(char);

// Go to the next line
void tty_nextRow();

// Write a fixed array of characters
void tty_writeN(const char*, size_t);

// Write a C style string to the TTY
void tty_writeString(const char*);