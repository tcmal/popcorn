// Buddy allocator system for frames of physical memory
// WIP
#include "physm_buddy.h"

const void* INITIAL_BUDDY_PAGE_ADDR = (void*) 0x42000000; // TODO: Allocate in our kernel vmem allocator

struct free_buddy {
	uint start;
	free_buddy_t* next;
};

struct dormant_buddy {
	dormant_buddy_t* next; // address of next dormant buddy in list, or 0 to use space immediately after
	
	// dormant_buddy_t and free_buddy MUST be the same size
	void* _a; 
};

// Get a dormant buddy to use
free_buddy_t* claim_buddy() {
	// Pop off the linked list
	free_buddy_t* ret = (free_buddy_t*) next_free_buddy;
	next_free_buddy = next_free_buddy->next;

	// Special case: null pointer just means use the next bit
	if (next_free_buddy == 0) {
		next_free_buddy = ret + 1;
		next_free_buddy->next = 0;

		if ((unsigned int) ret % ARCH_PAGE_SIZE >= ARCH_PAGE_SIZE - sizeof(dormant_buddy_t)) {
			kpanic("Unimplemented: Get another page for more physm buddies");
			// TODO
		}
	}

	return ret;
}

// Return a now-dormant buddy
void return_buddy(free_buddy_t* buddy) {
	// Add to linked list
	dormant_buddy_t* dormant = (dormant_buddy_t*) buddy;
	dormant->next = next_free_buddy;
	next_free_buddy = dormant;
}

// Should be called as early-on as possible.
void physm_init(uint kernel_end_address) {
	// Map an initial page for buddies
	void* next_free_page = ARCH_PAGE_SIZE * ((kernel_end_address / ARCH_PAGE_SIZE) + 1);
	int allocated = map_to_vmem(next_free_page, INITIAL_BUDDY_PAGE_ADDR, ARCH_PAGE_SIZE, PAGE_READ_WRITE);
	if (allocated < ARCH_PAGE_SIZE) {
		kpanic("couldn't map initial physm buddy page");
	}

	// Set up so we can claim from there
	next_free_buddy = INITIAL_BUDDY_PAGE_ADDR;
	next_free_buddy->next = 0;

	// Figure out how many pages we've already allocated
	uint last_page_used = (uint) next_free_page / ARCH_PAGE_SIZE;

	// Make our tree structure with that much already allocated
	// Create required children at each level
	uint lowest_buddy_made = -1;
	for (int order = PHYSM_MAX_BUDDY_ORDER; order >= 0; order--) {
		free_buddies[order] = 0;

		// Loop through all possible children that can exist at this level
		for (int pos = 0; pos < pow(2, PHYSM_MAX_BUDDY_ORDER - order); pos++) {
			uint start = pos * pow(2, order);
			if (start >= lowest_buddy_made) {
				// This and all past it are covered in higher-order buddies
				break;
			} else if (start > last_page_used) {
				// This area is free

				// Populate a buddy
				free_buddy_t* buddy = claim_buddy();
				buddy->start = start;
				buddy->next = free_buddies[order];

				free_buddies[order] = buddy;

				// Don't mark it as free again
				if (start < lowest_buddy_made)
					lowest_buddy_made = start;
			}
		}
	}
}

// Allocate a single page.
void* physm_alloc() {
	// Grab the smallest possible buddy
	free_buddy_t* buddy = 0;

	int order = 0;
	while (order <= PHYSM_MAX_BUDDY_ORDER) {
		if (free_buddies[order] != 0) {
			buddy = free_buddies[order];
			free_buddies[order] = buddy->next;

			break;
		}
		order++;
	}

	// TODO
	if ((int) buddy == 0) {
		kpanic("unimplemented: failed physalloc");
	}

	// Return buddy
	uint start_page = buddy->start;
	return_buddy(buddy);

	if (order > 0) {
		// Split larger buddy
		// We do this by repeatedly making right children until we get to order 0
		uint rightStart = start_page + pow(2, order - 1);
		for (int childOrder = order - 1; order > 0; order--) {
			free_buddy_t* rightChild = claim_buddy();

			rightChild->start = rightStart;
			rightStart -= pow(2, childOrder - 1);

			rightChild->next = free_buddies[childOrder];
			free_buddies[childOrder] = rightChild;
		}
	}

	return (void*)(start_page * ARCH_PAGE_SIZE);
}

// Attempts to recursively merge buddies using the given one
// If it can't, it just adds the buddy
void attempt_merge_buddy(uint start, int order) {
	uint orderSize = pow(2, order);

	// Get the start of the complimentary child
	bool isRightChild = start % pow(2, order + 1) != 0;
	uint complimentaryStart = isRightChild ? start - orderSize : start + orderSize;

	// Look for the complimentary buddy
	free_buddy_t* prevBuddy = 0;
	free_buddy_t* buddy = free_buddies[order];
	while (buddy != 0) {
		if (buddy->start == complimentaryStart) {
			// We can merge

			// Pop from list
			if (prevBuddy == 0) {
				free_buddies[order] = buddy->next;
			} else {
				prevBuddy->next = buddy->next;
			}

			return_buddy(buddy);

			// Recursively merge
			return attempt_merge_buddy(isRightChild ? complimentaryStart : start, order + 1);
		}

		prevBuddy = buddy;
		buddy = buddy->next;
	}


	// Otherwise, just add the buddy
	free_buddy_t* new = claim_buddy();
	new->start = start;

	// Push to list
	new->next = free_buddies[order];
	free_buddies[order] = new;
}

// Free the frame starting at start.
void physm_free(void* start) {
	uint pageNum = (uint)start / ARCH_PAGE_SIZE;

	return attempt_merge_buddy(pageNum, 0);
}


// Debug function to print current status
void physm_dump_status() {
	printf("PhysM status:\n");
	for (int order = 0; order <= PHYSM_MAX_BUDDY_ORDER; order++) {
		if (order < 10) {
			printf(" %u: ", order);
		} else {
			printf("%u: ", order);
		}

		free_buddy_t* buddy = free_buddies[order];
		while (buddy != 0) {
			printf("%u ", buddy->start);

			buddy = buddy->next;
		}
		printf("\n");
	}
}
