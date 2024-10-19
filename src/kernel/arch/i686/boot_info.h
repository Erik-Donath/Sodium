#pragma once
#include <stdint.h>

#define MEMORY_TYPE_MEMORY_AVAILABLE            1
#define MEMORY_TYPE_MEMORY_RESERVED		        2
#define MEMORY_TYPE_MEMORY_ACPI_RECLAIMABLE     3
#define MEMORY_TYPE_MEMORY_NVS                  4
#define MEMORY_TYPE_MEMORY_BADRAM               5

typedef struct {
    uint64_t basse_addr;
    uint64_t lenght;
    uint32_t type;
} INFO_memory_map_entry;

typedef struct {
    uint32_t mem_lower;
    uint32_t mem_upper;

    uint32_t mem_map_size;
    INFO_memory_map_entry mem_map[0];
} INFO_memory;

void INFO_ParseMultiboot(void* mb_info);
void* INFO_GetMultiboot();

// More funktions

/*
    Information about the multiboot info struct:
    http://www.brokenthorn.com/Resources/OSDevMulti.html
    --> https://www.gnu.org/software/grub/manual/multiboot2/html_node/Boot-information-format.html
    https://git.savannah.gnu.org/cgit/grub.git/tree/doc/multiboot2.h?h=multiboot2  
*/
