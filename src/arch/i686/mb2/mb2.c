#include "mb2.h"

#include <stdio.h>

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
    MB_TAG_IMAGE_LOAD_BASE_PHYSICAL_ADDRESS = 21
};

// It calculates the pointer of the tag that follows this tag by adding the size and aligning it (8byte alignment)
#define MB2_NEXT_TAG(tag) \
    (i686_mb2_tag_t*)(((uintptr_t)(tag) + (tag)->size + 7) & ~7)

typedef struct i686_mb2_tag_data_string {
    char string[0];
} __attribute__((packed)) i686_mb2_tag_data_string_t;

typedef struct i686_mb2_tag_data_32ptr {
    uint32_t ptr;
} __attribute__((packed)) i686_mb2_tag_data_32ptr_t;

typedef struct i686_mb2_tag_data_64ptr {
    uint64_t ptr;
} __attribute__((packed)) i686_mb2_tag_data_64ptr_t;

typedef struct i686_mb2_tag_data_memory_basic_info {
    uint32_t mem_lower;
    uint32_t mem_upper;
} __attribute__((packed)) i686_mb2_tag_data_memory_basic_info_t;

typedef struct i686_mb2_tag_data_memory_map {
    uint32_t entry_size;
    uint32_t entry_version;
    i686_mem_map_entry_t map[0];
} __attribute__((packed)) i686_mb2_tag_data_memory_map_t;

static i686_mem_info_t memory_info = (i686_mem_info_t){
    .lower = 0x00,
    .upper = 0x00,
    .entry_count = 0,
    .map = NULL,
};

bool i686_mb2_parse(i686_mb2_header_t* header) {
    uint8_t* end = (uint8_t*)header + header->total_size;
    i686_mb2_tag_t* tag = header->tags;

    enum {
        PARSE_REQ_MEM_BASIC_INFO = 0b00000001,
        PARSE_REQ_MEM_MAP        = 0b00000010,
        PARSE_REQ_ALL            = PARSE_REQ_MEM_BASIC_INFO | PARSE_REQ_MEM_MAP,
    };
    uint8_t required = 0;

    while ((uint8_t*)tag < end) {
        switch(tag->type) {
            case MB_TAG_BOOT_COMMAND_LINE: {
                i686_mb2_tag_data_string_t* str = (i686_mb2_tag_data_string_t*)tag->data;
                printf("[INFO] MB2 Boot Command line: %s\n", str->string);
            } break;
            case MB_TAG_BASIC_MEMORY_INFORMATION: {
                i686_mb2_tag_data_memory_basic_info_t* info = (i686_mb2_tag_data_memory_basic_info_t*)tag->data;
                memory_info.lower = info->mem_lower;
                memory_info.upper = info->mem_upper;

                required |= PARSE_REQ_MEM_BASIC_INFO;
            } break;
            case MB_TAG_MEMORY_MAP: {
                i686_mb2_tag_data_memory_map_t* map = (i686_mb2_tag_data_memory_map_t*)tag->data;
                memory_info.entry_count = (tag->size - sizeof(i686_mb2_tag_t) - sizeof(i686_mb2_tag_data_memory_map_t)) / map->entry_size;
                memory_info.map = (i686_mem_map_entry_t*)((uint8_t*)map + sizeof(i686_mb2_tag_data_memory_map_t)); // Calculate the map adress. #FIXME: This can be not aliged. Be carfull.
                
                required |= PARSE_REQ_MEM_MAP;
            } break;
            case MB_TAG_END_OF_MULTIBOOT_INFO: {
                bool failed = required != PARSE_REQ_ALL;
                if(failed)
                    printf("[ERR] MB2 Info does not contain required Entries. Missing: %#010b\n", (required ^ PARSE_REQ_ALL));
                return !failed;
            } break;
            default: {
                printf("[WARN] Failed to identify MB2 Tag %u with size %u\n", tag->type, tag->size);
            } break;
        }

        tag = MB2_NEXT_TAG(tag);
    }

    return false;
}

const i686_mem_info_t* i686_mb2_mem_info() {
    return &memory_info;
}
