#include <stdio.h>

#include <kernel/multiboot.h>
#include <kernel/tty.h>
#include <string.h>

void kernel_main(uint32_t** initialisedPageDirectory, uint32_t* mbInfo) {
	extern int _kernel_end;

	unsigned int kernel_end_address = (unsigned int) &_kernel_end;
	physm_init(kernel_end_address);

	setupPaging(initialisedPageDirectory);

	// TODO: Allocate pages for mbInfo, so we can actually read it.

	while (true) {};
}
