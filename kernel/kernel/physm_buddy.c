// Buddy allocator system for frames of physical memory
// WIP

#include <kernel/physm.h>
#include <kernel/tty.h>

#include <stdio.h>

// Maximum order of buddy to use
// Each buddy stores 2^n pages
#define MAX_ORDER 10

struct free_buddy {
	struct free_buddy* next;
	struct free_buddy* prev;
	unsigned int start;
};

// Array of circular linked lists of free buddies
// 0th element is an array of order 0 buddies (the smallest), and so on.
static struct free_buddy free_buddies[MAX_ORDER + 1];

void physm_init(unsigned int kernel_end_address) {
}