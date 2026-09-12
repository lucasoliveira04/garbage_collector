#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Allocator {
    void *endereco;
    size_t tamanho;
    struct Allocator *next;
} Allocator;

static Allocator *allocations = NULL;

// prototipos
void *gc_malloc(size_t size);
void gc_free(void *ptr);
void gc_register_allocation(void *ptr, size_t size);
void print_hp(void);

void *gc_malloc(size_t size) {
    void *ptr = malloc(size);

    if (ptr == NULL) {
        return NULL;
    }

    gc_register_allocation(ptr, size);

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

            free(curr->endereco);
            free(curr);

            return;
        }

        prev = curr;
        curr = curr->next;
    }
}


void gc_register_allocation(void *ptr, size_t size) {
    Allocator *alloc = malloc(sizeof(Allocator));

    if (alloc == NULL) {
        return;
    }

    alloc->endereco = ptr;
    alloc->tamanho = size;
    alloc->next = allocations;

    allocations = alloc;
}


void print_hp(void) {
    Allocator *atual = allocations;

    printf("Allocated memory heap:\n");

    while (atual != NULL) {
        printf(
            "Address: %p, Size: %zu\n",
            atual->endereco,
            atual->tamanho
        );

        atual = atual->next;
    }
}


int main(void) {
    int *number = gc_malloc(22);

    print_hp();

    gc_free(number);

    return 0;
}