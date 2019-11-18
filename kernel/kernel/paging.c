#include <kernel/paging.h>

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

void* getNextPage() {
	static size_t nextPageBoundary = 0; // 4MiB in. This leaves us space for all our paging tables.
	nextPageBoundary += 4096; // add 4 KiB
	return (void*) nextPageBoundary;
}

void pageDirectory_init(uint32_t* ptr) {
	for (int i = 0; i < 1024; i++) {
		ptr[i] = pageDirectoryEntry_new(0, false, 0);
	}
}

uint32_t pageDirectoryEntry_new(uint32_t* addr, bool present, int flags) {
	return (uint32_t) addr | flags | (present ? 1 : 0);
}

void pageTable_init_hollow(uint32_t* ptr) {
	for (int i = 0; i < 1024; i++) {
		ptr[i] = pageTableEntry_new(0, false, 0);
	}
}

void pageTable_init_filled(uint32_t* ptr, int flags) {
	for (int i = 0; i < 1024; i++) {
		ptr[i] = pageTableEntry_new(getNextPage(), true, flags);
	}
}

uint32_t pageTableEntry_new(void* addr, bool present, int flags) {
	return (uint32_t) addr | flags | (present ? 1 : 0);
}

void setupPaging() {
	// create a blank page directory
	uint32_t* page_directory = (uint32_t*) getNextPage();
	pageDirectory_init(page_directory);

	// create a page table
	// our first table has to cover the first 4MiB, so we don't automatically get new pages.
	uint32_t* page_table = (uint32_t*) getNextPage();
	for (int i = 0; i < 1024; i++) {
		page_table[i] = pageTableEntry_new((void*)(i * 0x1000), true, PDE_READ_WRITE);
	}

	// put the table in the directory
	page_directory[0] = pageDirectoryEntry_new(page_table, true, PDE_READ_WRITE);

	// set address of page table
	asm("mov %0, %%cr3" : : "r" (page_directory));

	// enable paging
	asm("mov %cr0, %eax;"
		"or $0x80000000, %eax;"
		"mov %eax, %cr0;");
}