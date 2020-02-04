#pragma once

#include <kernel/paging.h>

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

// Create a new entry for a page directory with the given options.
// addr must be 4KiB aligned, or 4MiB aligned if PDE_DIRECT_4MIB is set.
uint32_t pde_new(const void* addr, bool present, int flags);

// Create a new entry for a page table with the given options
// addr must be 4KiB aligned.
uint32_t pte_new(const void* addr, bool present, int flags);

// Get a pointer to the start of the page table `index`
// Returns a uint32_t[1024]. This will fall within the PT page.
uint32_t* getPt(uint32_t index);

// Get a pointer to the page directory entry at `index`
uint32_t* getPdEntry(uint32_t index);

// Clear/Initialise an area of memory for a pointer table.
// The pointer must be 4KiB aligned to make a valid PT.
// Every entry will be 'not present'
void pt_clear(uint32_t* ptr);

// Transform the ambiguous PAGE_* flags to flags for page directory entries
// (the equivalent PDE_* flags)
int toPdeFlags(int flags);