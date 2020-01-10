#include <kernel/paging.h>
#include <kernel/tty.h>
#include <stdio.h>

// Flags
const int PDE_DIRECT_4MIB = 1 << 7;
// bit 6 is always 0
const int PDE_ACCESSED = 1 << 5;
const int PDE_DISABLE_CACHE = 1 << 4;
const int PDE_WRITETHROUGH = 1 << 3;
const int PDE_USER = 1 << 2;
const int PDE_READ_WRITE = 1 << 1;

const int PTE_GLOBAL = 1 << 8;
// bit 7 is always 0
const int PTE_DIRTY = 1 << 6;
const int PTE_ACCESSED = 1 << 5;
const int PTE_DISABLE_CACHE = 1 << 4;
const int PTE_WRITETHROUGH = 1 << 3;
const int PTE_USER = 1 << 2;
const int PTE_READ_WRITE = 1 << 1;

// Static memory areas
// Both of these are initialised immediately in boot.S
// We set the addresses during preparePaging()
static uint32_t* page_directory;
static uint32_t* kernel_page_table;

// This is a single 4MiB page in which all the other page
// tables are stored. This is mapped during preparePaging().
const uint32_t* PT_PAGE = (uint32_t*) 0xbfc00000;

// The virtual address of the end of the kernel. This is used
// to figure out where to start mapping from.
extern int _kernel_end;
unsigned int kernel_end_address = (unsigned int) &_kernel_end;

uint32_t pde_new(void* addr, bool present, int flags) {
	return (uint32_t) addr | flags | (present ? 1 : 0);
}

uint32_t pte_new(void* addr, bool present, int flags) {
	return (uint32_t) addr | flags | (present ? 1 : 0);
}

void* getNextPage() {
	static size_t nextPageBoundary = 0x100000; // 4MiB in
	nextPageBoundary += 4096; // add 4 KiB
	return (void*) nextPageBoundary;
}

void clearPageCache() {
	asm("movl %cr3, %ecx");
	asm("movl %ecx, %cr3");
}

void preparePaging(uint32_t* initialisedPageDirectory) {

	page_directory = initialisedPageDirectory;
	kernel_page_table = page_directory[768] & 0xfffff000; // get rid of flag bits for raw address

	void* kernel_end_address_phys = kernel_end_address - 0xC0000000;
	void* pt_page_phys = (((unsigned int)kernel_end_address_phys / 0x400000) + 1) * 0x400000;
	page_directory[767] = pde_new(pt_page_phys, true, PDE_DIRECT_4MIB | PDE_READ_WRITE);

	clearPageCache();
}

void pt_clear(uint32_t* ptr) {
	for (int i = 0; i < 1024; i++) {
		ptr[i] = 0;
	}
}

// void assign_area_to(uint32_t phys, uint32_t virt, size_t size) {
// 	uint32_t page = phys / 0x1000;
// 	uint32_t alloc = size + (phys - page);
// 	while (true) {
// 		// TODO: Actually allocate page
// 		// setPageEntry(virt, pageTableEntry_new(page, true, PTE_READ_WRITE));

// 		if (alloc <= 0x1000) {
// 			break;
// 		}

// 		alloc -= 0x1000;
// 		page += 0x1000;
// 		virt += 0x1000;
// 	}
// }