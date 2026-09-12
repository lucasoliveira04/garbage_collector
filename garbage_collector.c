#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// gc_malloc: Allocates memory and tracks It for garbage collection
// gc_free: Free zone manually of memory
typedef struct Allocator{
    void *endereco;
    size_t tamanho;
    struct Allocator *next;
} Allocator;

static Allocator *allocations = NULL;

void *gc_malloc(size_t size) {
    void *ptr = malloc(size);

    if (ptr == NULL) return NULL;

    Allocator *alloc = malloc(sizeof(Allocator));

    alloc->endereco = ptr;
    alloc->tamanho = size;
    alloc->next = allocations;

    allocations = alloc;

    return ptr;
}

void gc_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    Allocator *curr = allocations;
    Allocator *prev = NULL;

    while (curr != NULL) {
        if (curr->endereco == ptr) {
            if (prev == NULL) {
                allocations = curr->next;
            } else {
                prev->next = curr->next;
            }
        }

        free(curr->endereco);
        free(curr);

        return;
    }

    prev = curr;
    curr = curr->next;
    printf("Memory heap after free");
} 

void print_hp(void) {
    Allocator *atual = allocations;

    printf("Alocated memory heap:\n");

    while (atual != NULL) {
        printf("Address: %p, Size: %zu\n", atual->endereco, atual->tamanho);
        atual = atual->next;
    }
}


int main() {
    int *number = gc_malloc(22);

    print_hp();

    gc_free(number);

    print_hp();

    return 0;
}