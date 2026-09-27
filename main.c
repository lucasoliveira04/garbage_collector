#include "garbage_collector.h"
#include "monitor.h"

#include <pthread.h>
#include <unistd.h>

int main(void) {

    pthread_t monitor_thread;

    pthread_create(
        &monitor_thread,
        NULL,
        gc_monitor,
        NULL
    );

    int *number = gc_malloc(sizeof(int));

    gc_add_root((void **)&number);

    sleep(3);

    int *numbers = gc_malloc(sizeof(int) * 100);

    gc_add_root((void **)&numbers);

    sleep(3);

    gc_remove_root((void **)&number);
    gc_free(number);

    sleep(3);

    gc_remove_root((void **)&numbers);
    gc_free(numbers);

    sleep(3);

    return 0;
}