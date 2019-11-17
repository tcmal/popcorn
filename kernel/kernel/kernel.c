#include <stdio.h>
#include <stdbool.h>

#include <kernel/tty.h>
#include <kernel/paging.h>
#include <string.h>


void kernel_main(uint32_t a) {
	tty_init();

	tty_writeString("Hello, World!");
	
	while (true) {};
}
