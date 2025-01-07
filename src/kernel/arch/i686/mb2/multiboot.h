#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <kernel/util.h>

typedef const void* mb_info_ptr;

// Copied from mb definition
enum {
    MEMORY_INFO_AVAILABLE           = 1,
    MEMORY_INFOY_RESERVED           = 2,
    MEMORY_INFO_ACPI_RECLAIMABLE    = 3,
    MEMORY_INFO_NVS                 = 4,
    MEMORY_INFO_BADRAM              = 5
};

typedef struct {
    uint64_t base_addr;
    uint64_t lenght;
    uint32_t type; // = memory_entry_type
    uint32_t reserved;
} PACKED memory_map_entry;

typedef struct {
    // Basic memory info
    uint32_t mem_lower;
    uint32_t mem_upper;

    // Memory Map
    uint32_t entry_count;
    memory_map_entry* entries; // WARN: Only valid if Bootloader is accessable / loaded !
} memory_info;

bool mb_parse(mb_info_ptr mb);
const memory_info* mb_getMemoryInfo();

void mb_print(mb_info_ptr mb);