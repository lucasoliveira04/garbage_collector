#include "monitor.h"
#include "garbage_collector.h"

#include <stdio.h>
#include <unistd.h>

void *gc_monitor(void *arg) {
    while (1) {
        size_t heap_usage = gc_get_heap_usage();
        size_t allocation_count = gc_get_allocation_count();

        printf(
            "[GC MONITOR] Heap: %zu bytes | Allocations: %zu\n",
            heap_usage,
            allocation_count
        );

        sleep(1);
    }

    return NULL;
}