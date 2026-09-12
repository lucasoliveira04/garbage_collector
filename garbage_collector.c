#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// gc_malloc: Allocates memory and tracks It for garbage collection


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

void print_hp(void) {
    Allocator *atual = allocations;

    printf("Alocated memory heap:\n");

    while (atual != NULL) {
        printf("Address: %p, Size: %zu\n", atual->endereco, atual->tamanho);
        atual = atual->next;
    }
}


int main() {
    int *number = gc_malloc(sizeof(int));
    *number = 22;
    
    char *buffer = gc_malloc(100);

    print_hp();

    return 0;
}