#include <kernel/paging.h>

static uint32_t** page_directory;
static uint32_t* kernel_page_table;

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
	static size_t nextPageBoundary = 0x100000; // 4MiB in
	nextPageBoundary += 4096; // add 4 KiB
	return (void*) nextPageBoundary;
}

// initialisedPageDirectory is initialised in assembly as we first start,
// So now we store its address properly so we can use it from C
void setupPaging(uint32_t** initialisedPageDirectory) {
	page_directory = initialisedPageDirectory;
	kernel_page_table = page_directory[768] & 0xfffff000; // get rid of flag bits for raw address
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