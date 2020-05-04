// Buddy allocator system for frames of physical memory
// WIP
#pragma once

// Max order of buddies
// Each buddy represents 2^order pages, so 11 is 2048 / 8MiB
#define PHYSM_MAX_BUDDY_ORDER 11

#include <arch_properties.h>

#include <stdlib.h>
#include <stdio.h>

#include <kernel/physm.h>
#include <kernel/paging.h>

const void* INITIAL_BUDDY_PAGE_ADDR;

// A buddy representing a free segment of memory
struct free_buddy;
typedef struct free_buddy free_buddy_t;

// An area of memory which could become a free_buddy
// This should be the same size as free_buddy
struct dormant_buddy;
typedef struct dormant_buddy dormant_buddy_t;

// Array of linked lists of free buddies
// 0th element is an array of order 0 buddies (the smallest), and so on.
static free_buddy_t* free_buddies[PHYSM_MAX_BUDDY_ORDER + 1];

// First element of linked list of dormant buddies
static dormant_buddy_t* next_free_buddy;

// Get a dormant buddy to use
free_buddy_t* claim_buddy();

// Return a now-dormant buddy
void return_buddy(free_buddy_t* buddy);

// Attempts to recursively merge buddies using the given one
// If it can't, it just adds the buddy
void attempt_merge_buddy(uint start, int order);

// Debug function to print current status
void physm_dump_status();