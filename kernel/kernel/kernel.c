#include <stdio.h>

#include <kernel/paging.h>
#include <kernel/multiboot.h>
#include <kernel/tty.h>

void kernel_main(uint32_t* initialisedPageDirectory, mbInfo_t* mbInfo) {
	preparePaging(initialisedPageDirectory);
	tty_init();

	// TODO: Allocate pages for mbInfo, so we can actually read it,
	//       Then pass info from it to physm to initialise that.

	printf("Hello, World!");
	while (true) {};
}
