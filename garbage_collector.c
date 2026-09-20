#include "garbage_collector.h"

#include <stdlib.h>
#include <pthread.h>

typedef struct Allocator {
    void *endereco;
    size_t tamanho;
    struct Allocator *next;
} Allocator;

static Allocator *allocations = NULL;

static pthread_mutex_t allocations_mutex =
    PTHREAD_MUTEX_INITIALIZER;


// private functions
static void gc_register_allocation(void *ptr, size_t size);
static void gc_unregister_allocation(void *ptr);


// allocate memory on heap
void *gc_malloc(size_t size) {

    void *ptr = malloc(size);

    if (ptr == NULL) {
        return NULL;
    }

    gc_register_allocation(ptr, size);

    return ptr;
}


// free memory
void gc_free(void *ptr) {

    if (ptr == NULL) {
        return;
    }

    gc_unregister_allocation(ptr);

    free(ptr);
}


static void gc_register_allocation(void *ptr, size_t size) {

    Allocator *alloc = malloc(sizeof(Allocator));

    if (alloc == NULL) {
        free(ptr);
        return;
    }

    alloc->endereco = ptr;
    alloc->tamanho = size;

    pthread_mutex_lock(&allocations_mutex);

    alloc->next = allocations;
    allocations = alloc;

    pthread_mutex_unlock(&allocations_mutex);
}


static void gc_unregister_allocation(void *ptr) {

    pthread_mutex_lock(&allocations_mutex);

    Allocator *curr = allocations;
    Allocator *prev = NULL;

    while (curr != NULL) {

        if (curr->endereco == ptr) {

            if (prev == NULL) {
                allocations = curr->next;
            } else {
                prev->next = curr->next;
            }

            free(curr);

            pthread_mutex_unlock(&allocations_mutex);

            return;
        }

        prev = curr;
        curr = curr->next;
    }

    pthread_mutex_unlock(&allocations_mutex);
}


// calculate how many bytes are currently registered
size_t gc_get_heap_usage(void) {

    size_t total = 0;

    pthread_mutex_lock(&allocations_mutex);

    Allocator *current = allocations;

    while (current != NULL) {
        total += current->tamanho;
        current = current->next;
    }

    pthread_mutex_unlock(&allocations_mutex);

    return total;
}


// Return how many allocations currently exist
size_t gc_get_allocation_count(void) {

    size_t count = 0;

    pthread_mutex_lock(&allocations_mutex);

    Allocator *current = allocations;

    while (current != NULL) {
        count++;
        current = current->next;
    }

    pthread_mutex_unlock(&allocations_mutex);

    return count;
}