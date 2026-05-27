#include "mb2.h"

#include "../mem/map.h"
#include "../mem/linker.h"

#include <string.h>
#include <stdio.h>

// Internal

enum {
    MB_TAG_END_OF_MULTIBOOT_INFO            = 0,
    MB_TAG_BOOT_COMMAND_LINE                = 1,
    MB_TAG_BOOTLOADER_NAME                  = 2,
    MB_TAG_MODULES                          = 3,
    MB_TAG_BASIC_MEMORY_INFORMATION         = 4,
    MB_TAG_BIOS_BOOT_DEVICE                 = 5,
    MB_TAG_MEMORY_MAP                       = 6,
    MB_TAG_VBE_INFO                         = 7,
    MB_TAG_FRAMEBUFFER_INFO                 = 8,
    MB_TAG_ELF_SYMBOLS                      = 9,
    MB_TAG_APM_TABLE                        = 10,
    MB_TAG_EFI_32_SYSTEM_TABLE_PTR          = 11,
    MB_TAG_EFI_64_SYSTEM_TABLE_PTR          = 12,
    MB_TAG_SMBIOS_TABLES                    = 13,
    MB_TAG_ACPI_OLD_RSDP                    = 14,
    MB_TAG_ACPI_NEW_RSDP                    = 15,
    MB_TAG_NETWORK_INFO                     = 16,
    MB_TAG_EFI_MEMORY_MAP                   = 17,
    MB_TAG_EFI_BOOT_SERVICES_NOT_TERMINATED = 18,
    MB_TAG_EFI_32_IMAGE_HANDLE_PTR          = 19,
    MB_TAG_EFI_64_IMAGE_HANDLE_PTR          = 20,
    MB_TAG_IMAGE_LOAD_BASE_PHYSICAL_ADDRESS = 21,
};

// @brief Advance to the next 8-byte-aligned tag.
#define MB2_NEXT_TAG(tag) \
    (i686_mb2_tag_t *)(((uintptr_t)(tag) + (tag)->size + 7) & ~7)

// @brief Results in the data part of the tag casted to the data type
#define MB2_TAG_DATA(tag, type) \
    (type*)((tag)->data)

typedef struct {
    char string[0];
} __attribute__((packed)) i686_mb2_tag_string_t;

typedef struct {
    uint32_t ptr;
} __attribute__((packed)) i686_mb2_tag_32ptr_t;

typedef struct {
    uint64_t ptr;
} __attribute__((packed)) i686_mb2_tag_64ptr_t;

typedef struct {
    uint32_t mem_lower;
    uint32_t mem_upper;
} __attribute__((packed)) i686_mb2_tag_mem_basic_t;

typedef struct {
    uint32_t             entry_size;
    uint32_t             entry_version;
    i686_mem_map_entry_t map[0];
} __attribute__((packed)) i686_mb2_tag_mem_map_t;

static i686_mem_info_t memory_info = {
    .lower       = 0,
    .upper       = 0,
    .phy_addr    = (uint32_t)(uintptr_t)&os_start,
    .entry_count = 0,
};

// Definitions

bool i686_mb2_parse(i686_mb2_header_t *header) {
    uint8_t        *end = (uint8_t *)header + header->total_size;
    i686_mb2_tag_t *tag = header->tags;

    enum {
        REQ_MEM_BASIC = 0x01,
        REQ_MEM_MAP   = 0x02,
        REQ_ALL       = REQ_MEM_BASIC | REQ_MEM_MAP,
    };
    uint8_t required = 0;

    while ((uint8_t *)tag < end) {
        switch (tag->type) {
        case MB_TAG_BOOT_COMMAND_LINE: {
            i686_mb2_tag_string_t *s = MB2_TAG_DATA(tag, i686_mb2_tag_string_t);
            printf("[INFO] MB2 Boot Command line: %s\n", s->string);
        } break;

        case MB_TAG_BASIC_MEMORY_INFORMATION: {
            i686_mb2_tag_mem_basic_t *m = MB2_TAG_DATA(tag, i686_mb2_tag_mem_basic_t);
            memory_info.lower = m->mem_lower;
            memory_info.upper = m->mem_upper;
            required |= REQ_MEM_BASIC;
        } break;

        case MB_TAG_MEMORY_MAP: {
            i686_mb2_tag_mem_map_t *map = MB2_TAG_DATA(tag, i686_mb2_tag_mem_map_t);
            if (map->entry_version != 0) {
                printf("[ERR] MB2 memory map uses unsupported version %u\n",
                       map->entry_version);
                return false;
            }
            size_t entry_count =
                (tag->size - sizeof(i686_mb2_tag_t) - sizeof(i686_mb2_tag_mem_map_t))
                / map->entry_size;
            if (entry_count > I686_MEM_MAP_MAX_ENTRIES) {
                printf("[ERR] MB2 memory map too large (%u > %u)\n",
                       entry_count, I686_MEM_MAP_MAX_ENTRIES);
                return false;
            }
            memory_info.entry_count = entry_count;
            memcpy(memory_info.map,
                   (uint8_t *)map + sizeof(i686_mb2_tag_mem_map_t),
                   entry_count * map->entry_size);
            required |= REQ_MEM_MAP;
        } break;

        case MB_TAG_IMAGE_LOAD_BASE_PHYSICAL_ADDRESS: {
            i686_mb2_tag_32ptr_t *a = MB2_TAG_DATA(tag, i686_mb2_tag_32ptr_t);
            memory_info.phy_addr = a->ptr;
        } break;

        case MB_TAG_END_OF_MULTIBOOT_INFO: {
            if (required != REQ_ALL) {
                printf("[ERR] MB2 missing required tags (have %#x, need %#x)\n",
                       required, REQ_ALL);
                return false;
            }
            return true;
        }

        default:
            printf("[WARN] Failed to identify MB2 Tag %u with size %u\n",
                   tag->type, tag->size);
            break;
        }

        tag = MB2_NEXT_TAG(tag);
    }

    return false;
}

const i686_mem_info_t *i686_mb2_get_mem_info(void) {
    return &memory_info;
}
