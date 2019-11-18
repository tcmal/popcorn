#include <stdio.h>

#include <kernel/tty.h>
#include <string.h>


void kernel_main() {
	extern int _kernel_end;

	unsigned int kernel_end_address = (unsigned int) &_kernel_end;
	// physm_init(kernel_end_address);

	tty_init();
	tty_writeString("Hello, World!");

	while (true) {};
}
