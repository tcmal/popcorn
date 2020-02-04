#include <stdio.h>

#include <kernel/paging.h>
#include <kernel/multiboot.h>
#include <kernel/tty.h>
#include <kernel/physm.h>

const void* MBINFO_PAGE_ADDRESS = (void*) 0x42000000;
extern int _kernel_end;

void kernel_main(uint32_t* initialisedPageDirectory, mbInfo_t* physMbInfo) {
	unsigned int kernel_end_address = (unsigned int) &_kernel_end;

	preparePaging(initialisedPageDirectory);
	tty_init();

	// Allocate pages for mbInfo, so we can actually read it,
	if (map_to_vmem((void*) physMbInfo, MBINFO_PAGE_ADDRESS, sizeof(mbInfo_t), PAGE_READ_WRITE) < sizeof(mbInfo_t)) {
		// TODO: Properly panic
		printf("FATAL: Couldn't map mbinfo into vmem\n");
		while (true) {};
	}
	mbInfo_t* mbInfo = MBINFO_PAGE_ADDRESS + ((unsigned int) physMbInfo % 0x1000); // May not be on page boundary
	// TODO: Initialise physm

	// printf("Kernel command line: %s\n", mbInfo->cmdline);
	printf("Reported lowest memory (KiB): %u\n", mbInfo->mem_lower);
	printf("Reported highest memory (KiB): %u\n", mbInfo->mem_upper);

	physm_init(kernel_end_address);
	
	while (true) {};
}
