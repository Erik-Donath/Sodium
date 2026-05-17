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

bool i686_mb2_parse(i686_mb2_header_t* header) {
    uint8_t* end = (uint8_t*)header + header->total_size;
    i686_mb2_tag_t* tag = header->tags;

    while ((uint8_t*)tag < end) {
        switch(tag->type) {
            case MB_TAG_END_OF_MULTIBOOT_INFO:
                return true;
            case MB_TAG_BOOT_COMMAND_LINE: {
                i686_mb2_tag_data_string_t* str = (i686_mb2_tag_data_string_t*)tag->data;
                printf("[INFO] MB2 Boot Command line: %s\n", str->string);
            } break;
            default:
                // Unknown MB2 Info so just skip it. #FIXME: Add Debugging
                break;
        }

        tag = MB2_NEXT_TAG(tag);
    }

    return false;
}
