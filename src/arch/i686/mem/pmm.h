#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "map.h"

// @brief Initialise the physical memory manager from a parsed memory map.
//        Places the page bitmap immediately after the kernel image. Reserves
//        the first 1 MiB, the kernel image, and the bitmap itself.
// @param info  Memory info from i686_mb2_get_mem_info().
// @return true on success; false if no region could hold the bitmap.
bool i686_mem_pmm_init(const i686_mem_info_t *info);

// @brief Allocate one 4 KiB physical page.
//        Pages below 1 MiB are never returned.
// @return Page-aligned physical address, or NULL if out of memory.
void *i686_mem_pmm_alloc(void);

// @brief Return a 4 KiB page to the PMM.
//        Ignores addresses below 1 MiB, inside the kernel image, or inside
//        the bitmap region.
// @param ptr  Address returned by i686_mem_pmm_alloc().
void i686_mem_pmm_free(void *ptr);
