
#include "paging.h"

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

// Ambiguous flags, for our case they are only transferred when written to a PDE.
const int PAGE_ACCESSED = PTE_ACCESSED;
const int PAGE_DISABLE_CACHE = PTE_DISABLE_CACHE;
const int PAGE_WRITETHROUGH = PTE_WRITETHROUGH;
const int PAGE_USER = PTE_USER;
const int PAGE_READ_WRITE = PTE_READ_WRITE;

// Static memory areas
// Both of these are initialised immediately in boot.S
// We set the addresses during preparePaging()
static uint32_t* page_directory;
static uint32_t* kernel_page_table;
static uint32_t* pt_page_phys;

// This is a single 4MiB page in which all the other page
// tables are stored. This is mapped during preparePaging().
uint32_t* const PT_PAGE = (uint32_t*) 0xbfc00000;

// The virtual address of the end of the kernel. This is used
// to figure out where to start mapping from.
extern int _kernel_end;
unsigned int kernel_end_address = (unsigned int) &_kernel_end;

uint32_t pde_new(const void* addr, bool present, int flags) {
	return (uint32_t) addr | flags | (present ? 1 : 0);
}

uint32_t pte_new(const void* addr, bool present, int flags) {
	return (uint32_t) addr | flags | (present ? 1 : 0);
}

void* getNextPage() {
	static size_t nextPageBoundary = 0x100000; // 4MiB in
	nextPageBoundary += 4096; // add 4 KiB
	return (void*) nextPageBoundary;
}

void pt_flush() {
	asm("movl %cr3, %ecx");
	asm("movl %ecx, %cr3");
}

void preparePaging(void* initialisedPageDirectory) {

	page_directory = (uint32_t*) initialisedPageDirectory;
	kernel_page_table = (uint32_t*) (page_directory[768] & 0xfffff000); // get rid of flag bits for raw address

	void* kernel_end_address_phys = (void*) (kernel_end_address - 0xC0000000);
	pt_page_phys = (uint32_t*) (((unsigned int) kernel_end_address_phys / 0x400000 + 1) * 0x400000);

	page_directory[767] = pde_new(pt_page_phys, true, PDE_DIRECT_4MIB | PDE_READ_WRITE);

	pt_flush();
}

uint32_t* getPt(uint32_t index) {
	return PT_PAGE + (index * 1024);
}

uint32_t* getPtPhys(uint32_t index) {
	return pt_page_phys + (index * 1024);
}

uint32_t* getPdEntry(uint32_t index) {
	return page_directory + index;
}

void pt_clear(uint32_t* ptr) {
	for (int i = 0; i < 1024; i++) {
		ptr[i] = 0;
	}
}

int toPdeFlags(int flags) {
	return flags & 0b111110;
}

int map_to_vmem(const void* phys, const void* virt, int size, int flags) {
	int page = (int) virt / 0x1000;
	int to_allocate = size + ((int) virt % 0x1000);
	int allocated = 0;
	phys = (void*) ((unsigned int) phys & 0xfffff000);
	
	while (to_allocate > 0) {
		int pdIndex = page / 1024;
		uint32_t* pde = getPdEntry(pdIndex);
		uint32_t* pt = getPt(pdIndex);

		if (*pde == 0) {
			// Set the PDE to point to the PT
			*pde = pde_new(getPtPhys(pdIndex), true, toPdeFlags(flags));

			pt_clear(pt);
		}

		if ((pt[page % 1024] & 1) != 0) {
			break;
		}
		pt[page % 1024] = pte_new(phys, true, flags);

		to_allocate -= 0x1000;
		allocated += 0x1000;
		page += 1;
		phys = phys + 0x1000;
	}

	pt_flush();

	return allocated;
}