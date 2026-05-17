#include "memory.h"

#include "linker.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

void i686_mem_init(const i686_mem_info_t* info) {
    uintptr_t os_start_addr = (uintptr_t)&os_start;
    uintptr_t os_end_addr = (uintptr_t)&os_end;

    printf(
        "[INFO] Memory Info:\n\tlower \t = %#x\n\tupper \t = %#x\n\tphy_addr = %#x\n\tos_start = %#x\n\tos_end \t = %#x\n", 
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
}
