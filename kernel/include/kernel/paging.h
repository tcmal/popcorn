#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Flags
const int PAGE_ACCESSED;
const int PAGE_DISABLE_CACHE;
const int PAGE_WRITETHROUGH;
const int PAGE_USER;
const int PAGE_READ_WRITE;

// Refreshes the page directory and page tables.
void pt_flush();

// Perform initial paging setup.
// By the end of this, we should be able to allocate any arbritary page.
void preparePaging(void* initialisedPageDirectory);

// Map the given area of physical memory to virtual memory, starting at `virt`
// Returns the number of bytes allocated, which may not equal `size` and may be 0.
// This stops allocating when it encounters an already allocated page
// Flags uses the PAGE_* flags.
int map_to_vmem(void* phys, void* virt, int size, int flags);