#include "kernel.h"

#include "memory/heap.h"

#include <stdint.h>
#include <stdio.h>

// Definitions

void k_main(void) {
    k_heap_init();
}
