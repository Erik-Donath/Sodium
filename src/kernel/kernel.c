#include "kernel.h"

#include "memory/heap.h"

#include <stdint.h>
#include <stdio.h>

void k_main(void) {
    k_heap_init();
    printf("Kernel loaded\n");

    uint64_t count = 0;
    void* last = NULL;
    void* this = NULL;

    for(size_t i = 0; i < 2; i++) {
        while((this = k_heap_alloc(sizeof(void*)))) {
            *(void**)this = last;
            last  = this;
            count++;
        }
        printf("Took %llu allocations\n", count);

        while (last) {
            this = last;
            void *next = *(void **)this;
            k_heap_free(this);
            last = next;
            count--;
        }
        if(count) {
            printf("We did not free all allocs: %llu\n", count);
        }

    }
    return;
}
