#include "garbage_collector.h"

#include <stdlib.h>
#include <pthread.h>

typedef struct Allocator {
    void *endereco;
    size_t tamanho;
    struct Allocator *next;
} Allocator;

typedef struct Root {
    void **ptr;
    struct Root *next;
} Root;

static Root *roots = NULL;
static Allocator *allocations = NULL;

static pthread_mutex_t allocations_mutex =
    PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t roots_mutex =
    PTHREAD_MUTEX_INITIALIZER;


// private functions
static void gc_register_allocation(void *ptr, size_t size);
static void gc_unregister_allocation(void *ptr);
static size_t existRoot(void **root);

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

void gc_add_root(void **root) {

    if (root == NULL) {
        return;
    }

    Root *new_root = malloc(sizeof(Root));

    if (new_root == NULL) {
        return;
    }

    new_root->ptr = root;

    pthread_mutex_lock(&roots_mutex);
    
    if (existRoot(root)) {
        pthread_mutex_unlock(&roots_mutex);
        return;
    }

    new_root->next = roots;
    roots = new_root;

    pthread_mutex_unlock(&roots_mutex);
}

void gc_remove_root(void **root) {
    if (root == NULL) {
        return;
    }

    pthread_mutex_lock(&roots_mutex);

    Root *curr = roots;
    Root *prev = NULL;

    while (curr != NULL) {
        if (curr->ptr == root) {
            if (prev == NULL) {
                roots = curr->next;
            } else {
                prev->next = curr->next;
            }

            free(curr);

            pthread_mutex_unlock(&roots_mutex);

            return;
        }

        prev = curr;
        curr = curr->next;
    }

    pthread_mutex_unlock(&roots_mutex);
}

static size_t existRoot(void **root) {
    Root *curr = roots;

    while (curr != NULL) {
        if (curr->ptr == root) {
            return 1;
        } 

        curr = curr->next;
    }

    return 0;

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