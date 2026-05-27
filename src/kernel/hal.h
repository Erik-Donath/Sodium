#pragma once

// Hardware Abstraction Layer.
// The architecture-specific layer (src/arch/i686/hal.c) implements these.
// The high kernel only touches hardware through these functions.
//
// Future: most hardware interaction will go through the Driver Management
// System (DMS). HAL will grow to support driver registration and bus discovery.

// @brief Write one character to the debug output channel.
//        Debug builds: port 0xE9 (QEMU debugcon). Release builds: no-op.
void hal_debug_putc(char c);

// @brief Allocate one 4 KiB physical page.
// @return Page-aligned physical address, or NULL if out of memory.
void *hal_phy_page_alloc(void);

// @brief Return a 4 KiB page to the PMM.
// @param ptr  Page-aligned address returned by hal_phy_page_alloc().
void hal_phy_page_free(void *ptr);
