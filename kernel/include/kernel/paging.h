#ifndef _KERNEL_PAGING_H
#define _KERNEL_PAGING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Flags

// If this is set, the address is a direct pointer to a 4MiB page (and is 4MiB aligned).
const int PDE_DIRECT_4MIB;
const int PDE_ACCESSED;
const int PDE_DISABLE_CACHE;
const int PDE_WRITETHROUGH;
const int PDE_USER;
const int PDE_READ_WRITE;


// If this is set, this PTE won't be cleared from the cache when the PT is reset.
const int PTE_GLOBAL;
const int PTE_DIRTY;
const int PTE_ACCESSED;
const int PTE_DISABLE_CACHE;
const int PTE_WRITETHROUGH;
const int PTE_USER;
const int PTE_READ_WRITE;


// Return a pointer for a 4KiB aligned page.
void* getNextPage();

// Make a new Page Directory, aka uint32_t[1024].
// The pointer must be 4KiB aligned to make a valid PD.
// Every entry will be 'not present'
void pageDirectory_init_empty(uint32_t* ptr);

// Create a new entry for a page directory with the given options.
// addr must be 4KiB aligned.
uint32_t pageDirectoryEntry_new(uint32_t* addr, bool present, int flags);

// Make a new page table, aka uint32_t[1024].
// The pointer must be 4KiB aligned to make a valid PT.
// Every entry will be 'not present'
void pageTable_init_hollow(uint32_t* ptr);

// Make a new page table, aka uint32_t[1024].
// The pointer must be 4KiB aligned to make a valid PT.
// Every entry will be present with `flags`.
void pageTable_init_filled(uint32_t* ptr, int flags);

// Create a new entry for a page table with the given options
// addr must be 4KiB aligned.
uint32_t pageTableEntry_new(void* addr, bool present, int flags);

// Perform initial paging setup.
void setupPaging(uint32_t**);

#endif