#pragma once

#include <stdint.h>
#include <stddef.h>

// Multiboot2 / BIOS-e820 memory region types.
enum {
    MEMORY_INFO_AVAILABLE        = 1,
    MEMORY_INFO_RESERVED         = 2,
    MEMORY_INFO_ACPI_RECLAIMABLE = 3,
    MEMORY_INFO_NVS              = 4,
    MEMORY_INFO_BADRAM           = 5,
};

typedef struct i686_mem_map_entry {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} i686_mem_map_entry_t;

#define I686_MEM_MAP_MAX_ENTRIES  128

typedef struct i686_mem_info {
    uint32_t             lower;
    uint32_t             upper;
    uint32_t             phy_addr;
    size_t               entry_count;
    i686_mem_map_entry_t map[I686_MEM_MAP_MAX_ENTRIES];
} i686_mem_info_t;
