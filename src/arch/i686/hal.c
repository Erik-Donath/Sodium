#include <kernel/hal.h>

#include "debug.h"
#include "mem/pmm.h"

// Debug putc
void hal_debug_putc(char c) {
    i686_debug_putc(c);
}

// Physicale Pages
void* hal_phy_page_alloc(void) {
    return i686_mem_pmm_alloc();
}

void hal_phy_page_free(void* ptr) {
    i686_mem_pmm_free(ptr);
}
