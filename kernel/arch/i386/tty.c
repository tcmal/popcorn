#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

#include <kernel/tty.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xC03FF000; // This is where we've mapped it virtually

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

// Helper function
uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

// TODO: Move this
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

uint8_t tty_format(ttycolor_t fg, ttycolor_t bg) 
{
	return fg | bg << 4;
}

void tty_init()
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = tty_format(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) VGA_MEMORY;
	// Clear the buffer. Technically this is optional.
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

// ttycolor is based on VGA, so this is always true.
bool tty_supportsFormat(__attribute__((unused)) uint8_t _) {
	return true;
}

void tty_setFormat(uint8_t format) {
	terminal_color = format;
}

void tty_charAt(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void tty_putChar(char c) 
{
	if (c == 0xa) {
		tty_nextRow();
		return;
	}
	tty_charAt(c, terminal_color, terminal_column, terminal_row);
	// Wrap around to next character
	if (++terminal_column == VGA_WIDTH) {
		tty_nextRow();
	}
}


void tty_nextRow() {
	terminal_column = 0;
	terminal_row++;
	if (terminal_row == VGA_HEIGHT) {
		// copy all rows backwards
		for (size_t i = 1; i < VGA_HEIGHT; i++) {
			memcpy(&terminal_buffer[(i-1) * VGA_WIDTH], &terminal_buffer[i * VGA_WIDTH], VGA_WIDTH);
		}
		terminal_row--;
	}
}

void tty_writeN(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		tty_putChar(data[i]);
}

void tty_writeString(const char* data) {
	tty_writeN(data, strlen(data));
}
