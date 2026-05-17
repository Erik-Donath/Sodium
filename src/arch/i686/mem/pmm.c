#include "pmm.h"

#include "map.h"
#include "linker.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static uint32_t* pmm_bitmap = NULL;
static size_t  pmm_total_pages = 0;

bool i686_mem_pmm_init(const i686_mem_info_t* info) {
    uintptr_t os_start_addr = (uintptr_t)&os_start;
    uintptr_t os_end_addr = (uintptr_t)&os_end;

    // Print Memory Info and Memory Map
    printf(
        "[INFO] Memory Info:\n\tlower \t = %#010x\n\tupper \t = %#010x\n\tphy_addr = %#010x\n\tos_start = %#010x\n\tos_end \t = %#010x\n", 
        info->lower, info->upper, info->phy_addr, os_start_addr, os_end_addr
    );
    printf("[INFO] Memory Map(%u): \n", info->entry_count);
    for(size_t i = 0; i < info->entry_count; i++) {
        i686_mem_map_entry_t* entry = &info->map[i];
        printf(
            "\t%u: {type = %u, base = %#018llx, length = %#018llx}\n",
            i, entry->type, (unsigned long long)entry->base_addr, (unsigned long long)entry->length
        );
    }

    // Find highest address posible and the os entry
    uint64_t top = 0;
    uint32_t os_entry = info->entry_count; // Invalid Entry before start
    for(size_t i = 0; i < info->entry_count; i++) {
        if(info->map[i].type != MEMORY_INFO_AVAILABLE) continue;
        uint64_t start = info->map[i].base_addr;
        uint64_t end = start + info->map[i].length;
        if(end > top)
            top = end;
        if((uint64_t)os_start_addr >= start && (uint64_t)os_end_addr <= end)
            os_entry = i;
    }

    // Calculate Page count and Bitmap size
    size_t total_pages = (size_t)(top / 4096);
    size_t bitmap_bytes = (total_pages + 7) / 8;
    printf("[DINFO] Highest available address: %#018llx, Total Pages: %zu, Bitmap Bytes: %zu, OS Entry: %u\n", top, total_pages, bitmap_bytes, os_entry);

    // Find location for pmm bitmap. Prefer using the os entry
    uint32_t os_bitmap_addr = (uint32_t)((os_end_addr + 4095) & ~4095);
    if((uint64_t)os_bitmap_addr + bitmap_bytes <= info->map[os_entry].base_addr + info->map[os_entry].length) {
        pmm_bitmap = (uint32_t*)os_bitmap_addr;
    }
    else {
        for(size_t i = 0; i < info->entry_count; i++) {
            if(i == os_entry) continue;
            if(info->map[i].type != MEMORY_INFO_AVAILABLE) continue;
            if(info->map[i].base_addr == 0x00) continue;
            if(info->map[i].length < bitmap_bytes) continue;

            pmm_bitmap = (uint32_t*)(uintptr_t)info->map[i].base_addr;
            break;
        }
    }

    if(!pmm_bitmap) {
        printf("[ERR] Failed to find fitting Region for PMM\n");
        return false;
    }

    pmm_total_pages = total_pages;
    printf("[DINFO] PMM Bitmap placed at %#010x\n", (uint32_t)(uintptr_t)pmm_bitmap);

    // Reserve everything
    memset(pmm_bitmap, 0xFF, bitmap_bytes);

    // #TODO:
    // Walk Map and free ever available entry
    // Mark OS Page(s)
    // Mark Bitmap Page(s)

    return true;
}
