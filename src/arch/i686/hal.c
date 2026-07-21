#include <kernel/hal.h>

#include "debug.h"
#include "mem/pmm.h"
#include "mem/vmm.h"

// Definitions

void hal_debug_putc(char c) {
    i686_debug_putc(c);
}

void *hal_phy_page_alloc(void) {
    void *phys = i686_mem_pmm_alloc();
    return phys ? PHYS_TO_VIRT(phys) : NULL;
}

void hal_phy_page_free(void *ptr) {
    if (!ptr) return;
    i686_mem_pmm_free(VIRT_TO_PHYS(ptr));
}
