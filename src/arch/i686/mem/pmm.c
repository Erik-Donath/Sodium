#include "pmm.h"

#include "map.h"
#include "linker.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// Internal

#define PMM_PAGE_SIZE    4096u
#define PMM_WORD_SIZE    32u
#define PMM_LOW_MEM_END  0x100000u
#define PMM_4GB          0x100000000ull

static uint32_t *pmm_bitmap      = NULL;
static size_t    pmm_bitmap_bytes = 0;
static size_t    pmm_total_pages  = 0;
static size_t    pmm_hint_word    = 0;

// Cached linker symbols as plain integers (avoids repeated pointer casts).
static const uintptr_t pmm_os_start = (uintptr_t)&os_start;
static const uintptr_t pmm_os_end   = (uintptr_t)&os_end;

// @brief Mark a page as allocated (1 = used). No-op if out of range.
static inline void i686_mem_pmm_set(uint32_t page);

// @brief Mark a page as free (0 = free). No-op if out of range.
static inline void i686_mem_pmm_clear(uint32_t page);

// @brief Return true if the page is allocated or out of range.
static inline bool i686_mem_pmm_test(uint32_t page);

// @brief Mark every page in [base, base+length) as allocated.
//        Clamps silently to 4 GiB.
static void i686_mem_pmm_region_reserve(uint64_t base, uint64_t length);

// @brief Mark every page in [base, base+length) as free.
//        Silently clamps or skips ranges beyond 4 GiB.
static void i686_mem_pmm_region_free(uint64_t base, uint64_t length);

// Definitions

static inline void i686_mem_pmm_set(uint32_t page) {
    if (page < pmm_total_pages)
        pmm_bitmap[page / PMM_WORD_SIZE] |= (1u << (page % PMM_WORD_SIZE));
}

static inline void i686_mem_pmm_clear(uint32_t page) {
    if (page < pmm_total_pages)
        pmm_bitmap[page / PMM_WORD_SIZE] &= ~(1u << (page % PMM_WORD_SIZE));
}

static inline bool i686_mem_pmm_test(uint32_t page) {
    if (page < pmm_total_pages)
        return (pmm_bitmap[page / PMM_WORD_SIZE] >> (page % PMM_WORD_SIZE)) & 1u;
    return true;
}

static void i686_mem_pmm_region_reserve(uint64_t base, uint64_t length) {
    if (length == 0) return;
    if (base >= PMM_4GB) return;
    if (base + length > PMM_4GB) length = PMM_4GB - base;

    size_t first = (size_t)(base / PMM_PAGE_SIZE);
    size_t last  = (size_t)((base + length + PMM_PAGE_SIZE - 1) / PMM_PAGE_SIZE);
    for (size_t p = first; p < last; p++)
        i686_mem_pmm_set((uint32_t)p);
}

static void i686_mem_pmm_region_free(uint64_t base, uint64_t length) {
    if (base >= PMM_4GB) return;
    if (base + length > PMM_4GB) length = PMM_4GB - base;
    if (length == 0) return;

    size_t first = (size_t)(base / PMM_PAGE_SIZE);
    size_t last  = (size_t)((base + length + PMM_PAGE_SIZE - 1) / PMM_PAGE_SIZE);
    for (size_t p = first; p < last; p++)
        i686_mem_pmm_clear((uint32_t)p);
}

bool i686_mem_pmm_init(const i686_mem_info_t *info) {
    printf("[INFO] Memory Info:\n"
           "\tlower    = %#010x\n\tupper    = %#010x\n"
           "\tphy_addr = %#010x\n\tos_start = %#010x\n\tos_end   = %#010x\n",
           info->lower, info->upper, info->phy_addr,
           (uint32_t)pmm_os_start, (uint32_t)pmm_os_end);

    printf("[INFO] Memory Map(%u): \n", info->entry_count);
    for (size_t i = 0; i < info->entry_count; i++) {
        const i686_mem_map_entry_t *e = &info->map[i];
        printf("\t%u: {type = %u, base = %#018llx, length = %#018llx}\n",
               i, e->type,
               (unsigned long long)e->base_addr,
               (unsigned long long)e->length);
    }

    uint64_t top      = 0;
    uint32_t os_entry = (uint32_t)info->entry_count;

    for (size_t i = 0; i < info->entry_count; i++) {
        if (info->map[i].type != MEMORY_INFO_AVAILABLE) continue;
        uint64_t start = info->map[i].base_addr;
        if (start >= PMM_4GB) continue;
        uint64_t end = start + info->map[i].length;
        if (end > PMM_4GB) end = PMM_4GB;
        if (end > top) top = end;
        if ((uint64_t)pmm_os_start >= start && (uint64_t)pmm_os_end <= end)
            os_entry = (uint32_t)i;
    }

    if (os_entry == info->entry_count) {
        printf("[ERR] Failed to find OS entry in memory map\n");
        return false;
    }

    size_t total_pages  = (size_t)(top / PMM_PAGE_SIZE);
    size_t bitmap_bytes = (total_pages + 7) / 8;
    printf("[INFO] Highest available Address: %#018llx, Total Pages: %zu\n",
           top, total_pages);

    // Prefer placing the bitmap right after the kernel image.
    uintptr_t os_bitmap = (uintptr_t)((pmm_os_end + PMM_PAGE_SIZE - 1)
                          & ~(PMM_PAGE_SIZE - 1));
    if ((uint64_t)os_bitmap + bitmap_bytes <=
        info->map[os_entry].base_addr + info->map[os_entry].length) {
        pmm_bitmap = (uint32_t *)os_bitmap;
    } else {
        for (size_t i = 0; i < info->entry_count; i++) {
            if (i == os_entry) continue;
            if (info->map[i].type != MEMORY_INFO_AVAILABLE) continue;
            if (info->map[i].base_addr == 0) continue;
            if (info->map[i].length < bitmap_bytes) continue;
            pmm_bitmap = (uint32_t *)(uintptr_t)info->map[i].base_addr;
            break;
        }
    }

    if (!pmm_bitmap) {
        printf("[ERR] Failed to find region for PMM bitmap\n");
        return false;
    }

    pmm_bitmap_bytes = bitmap_bytes;
    pmm_total_pages  = total_pages;
    printf("[INFO] PMM Bitmap placed at %#010x\n",
           (uint32_t)(uintptr_t)pmm_bitmap);

    memset(pmm_bitmap, 0xFF, bitmap_bytes);  // all pages reserved

    for (size_t i = 0; i < info->entry_count; i++)
        if (info->map[i].type == MEMORY_INFO_AVAILABLE)
            i686_mem_pmm_region_free(info->map[i].base_addr,
                                     info->map[i].length);

    i686_mem_pmm_region_reserve(0, PMM_LOW_MEM_END);
    i686_mem_pmm_region_reserve(pmm_os_start, pmm_os_end - pmm_os_start);
    i686_mem_pmm_region_reserve((uint64_t)(uintptr_t)pmm_bitmap,
                                 (uint64_t)bitmap_bytes);

    pmm_hint_word = PMM_LOW_MEM_END / PMM_PAGE_SIZE / PMM_WORD_SIZE;
    return true;
}

void *i686_mem_pmm_alloc(void) {
    size_t words = (pmm_total_pages + PMM_WORD_SIZE - 1) / PMM_WORD_SIZE;
    for (size_t w = pmm_hint_word; w < words; w++) {
        if (pmm_bitmap[w] == 0xFFFFFFFF) continue;
        for (uint8_t bit = 0; bit < PMM_WORD_SIZE; bit++) {
            uint64_t page = (uint64_t)w * PMM_WORD_SIZE + bit;
            if (page >= pmm_total_pages) return NULL;
            if (!i686_mem_pmm_test((uint32_t)page)) {
                i686_mem_pmm_set((uint32_t)page);
                pmm_hint_word = w;
                return (void *)(uintptr_t)(page * PMM_PAGE_SIZE);
            }
        }
    }
    return NULL;
}

void i686_mem_pmm_free(void *ptr) {
    uintptr_t addr = (uintptr_t)ptr;
    if (addr < PMM_LOW_MEM_END) return;
    if (addr >= pmm_os_start && addr < pmm_os_end) return;
    if (addr >= (uintptr_t)pmm_bitmap &&
        addr <  (uintptr_t)pmm_bitmap + pmm_bitmap_bytes) return;

    uint32_t page = (uint32_t)(addr / PMM_PAGE_SIZE);
    i686_mem_pmm_clear(page);

    uint32_t word = page / PMM_WORD_SIZE;
    if (word < pmm_hint_word) pmm_hint_word = word;
}
