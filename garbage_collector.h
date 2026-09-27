#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

#include <stddef.h>

// Allocates memory and registers the allocation in the GC
void *gc_malloc(size_t size);

// Removes the allocation from the GC and frees the memory
void gc_free(void *ptr);

// Returns the total amount of memory currently allocated
size_t gc_get_heap_usage(void);

// Returns the number of active allocations
size_t gc_get_allocation_count(void);

void gc_add_root(void **root);
void gc_remove_root(void **root);

#endif