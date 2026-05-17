#pragma once
#include <stdint.h>
#include <stddef.h>

enum {
    MEMORY_INFO_AVAILABLE           = 1,
    MEMORY_INFO_RESERVED            = 2,
    MEMORY_INFO_ACPI_RECLAIMABLE    = 3,
    MEMORY_INFO_NVS                 = 4,
    MEMORY_INFO_BADRAM              = 5
};

typedef struct i686_mem_map_entry {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} i686_mem_map_entry_t;

typedef struct i686_mem_info {
    uint32_t lower;
    uint32_t upper;
    size_t   entry_count;
    i686_mem_map_entry_t* map;
} i686_mem_info_t;

// The mem info struct and map musst be available when calling the function. After the call the memory info is stored.
void i686_mem_init(const i686_mem_info_t* info);

