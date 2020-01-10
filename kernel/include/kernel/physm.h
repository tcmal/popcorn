#pragma once

// Physical Memory Manager (Implementation agnostic header)
// This can be used to allocate physical memory frames of 4KiB.
// Makes no promises about continuity of each frame.

// Should be called as early-on as possible.
void physm_init(unsigned int kernel_end_address);

// Allocate a single page.
void* physm_alloc();

// Allocate n pages. This lets the allocator use optimisations if many are requested at once.
// dest should be a void*[n].
// No promises about continuity.
// Returns the number of allocated frames. This might not always be n.
int physm_alloc_n(int n, void** dest);

// Free the frame starting at start.
int physm_free(void* start);