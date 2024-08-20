#include <stdint.h>
#include "util/assembly.h"
#include "stdio.h"
#include "sysinfo.h"

#define MB_TAG_END_OF_MULTIBOOT_INFO            0
#define MB_TAG_BOOT_COMMAND_LINE                1
#define MB_TAG_BOOTLOADER_NAME                  2
#define MB_TAG_MODULES                          3
#define MB_TAG_BASIC_MEMORY_INFORMATION         4
#define MB_TAG_BIOS_BOOT_DEVICE                 5
#define MB_TAG_MEMORY_MAP                       6
#define MB_TAG_VBE_INFO                         7
#define MB_TAG_FRAMEBUFFER_INFO                 8
#define MB_TAG_ELF_SYMBOLS                      9
#define MB_TAG_APM_TABLE                        10
#define MB_TAG_EFI_32_SYSTEM_TABLE_PTR          11
#define MB_TAG_EFI_64_SYSTEM_TABLE_PTR          12
#define MB_TAG_SMBIOS_TABLES                    13
#define MB_TAG_ACPI_OLD_RSDP                    14
#define MB_TAG_ACPI_NEW_RSDP                    15
#define MB_TAG_NETWORK_INFO                     16
#define MB_TAG_EFI_MEMORY_MAP                   17
#define MB_TAG_EFI_BOOT_SERVICES_NOT_TERMINATED 18
#define MB_TAG_EFI_32_IMAGE_HANDLE_PTR          19
#define MB_TAG_EFI_64_IMAGE_HANDLE_PTR          20
#define MB_TAG_IMAGE_LOAD_BASE_PHYSICAL_ADDRESS 21

#define MB_STRUCT_TAG_ALIGN 8
#define MB_STRUCT_ALIGN_UP(value, align) (((value) + (align) - 1) & ~((align - 1)))
#define MB_STRUCT_NEXT_TAG(tag) (multiboot_tag*)((uint8_t*)tag + MB_STRUCT_ALIGN_UP(tag->size, MB_STRUCT_TAG_ALIGN))

/* Not needed */
#define MB_TAG_UNKNOWN "MB_TAG_UNKNOWN"
static const char* mb_info_tag_name_table[23] = {
    "MB_TAG_END_OF_MULTIBOOT_INFO",
    "MB_TAG_BOOT_COMMAND_LINE",
    "MB_TAG_BOOTLOADER_NAME",
    "MB_TAG_MODULES",
    "MB_TAG_BASIC_MEMORY_INFORMATION",
    "MB_TAG_BIOS_BOOT_DEVICE",
    "MB_TAG_MEMORY_MAP",
    "MB_TAG_VBE_INFO",
    "MB_TAG_FRAMEBUFFER_INFO",
    "MB_TAG_ELF_SYMBOLS",
    "MB_TAG_APM_TABLE",
    "MB_TAG_EFI_32_SYSTEM_TABLE_PTR",
    "MB_TAG_EFI_64_SYSTEM_TABLE_PTR",
    "MB_TAG_SMBIOS_TABLES",
    "MB_TAG_ACPI_OLD_RSDP",
    "MB_TAG_ACPI_NEW_RSDP",
    "MB_TAG_NETWORK_INFO",
    "MB_TAG_EFI_MEMORY_MAP",
    "MB_TAG_EFI_BOOT_SERVICES_NOT_TERMINATED",
    "MB_TAG_EFI_32_IMAGE_HANDLE_PTR",
    "MB_TAG_EFI_64_IMAGE_HANDLE_PTR",
    "MB_TAG_IMAGE_LOAD_BASE_PHYSICAL_ADDRESS"
};

/* MULTIBOOT INFO HEADER and TAG HEADER */
typedef struct {
    uint32_t type;
    uint32_t size;
} ASMPACK multiboot_tag;

typedef struct {
    uint32_t total_size;
    uint32_t reserved;
    multiboot_tag tags[0];
} ASMPACK multiboot_header;

/* MULTIBOOT TAG STRUCTS*/

typedef struct {
    multiboot_tag head;
    char string[0];
} ASMPACK multiboot_tag_command_line;

typedef struct {
    multiboot_tag head;
    uint32_t mem_lower;
    uint32_t mem_upper;
} ASMPACK multiboot_tag_basic_meminfo;

void SYSINFO_parse_multiboot(void* mb_info) {
    multiboot_header* header = (multiboot_header*)mb_info;
    printf("Multiboot Info:\n");

    if(header->reserved) {
        printf("Error reserved is not zero.\n\treserved_value = %u\n", header->reserved);
        return;
    }

    multiboot_tag* tag = header->tags;
    while((uint8_t*)tag < (uint8_t*)header + header->total_size) {
        const char* tag_name = MB_TAG_UNKNOWN;
        if(tag->type < 23)
            tag_name = mb_info_tag_name_table[tag->type];
        
        printf("Entry at %p %s(%u) with %u bytes\n", tag, tag_name, tag->type, tag->size);
        switch(tag->type) {
            case MB_TAG_END_OF_MULTIBOOT_INFO:
                printf("\t End of Multiboot info struct.\n");
                return;
            case MB_TAG_BOOT_COMMAND_LINE:
                multiboot_tag_command_line* line = (multiboot_tag_command_line*)tag;
                printf("\tstring = \"%s\"\n", line->string);
                break;
            case MB_TAG_BASIC_MEMORY_INFORMATION:
                multiboot_tag_basic_meminfo* meminfo = (multiboot_tag_basic_meminfo*)tag;
                printf("\tmem_lower = %u\n\tmem_upper = %u\n", meminfo->mem_lower, meminfo->mem_upper);
                break;
            default:
                //printf("\t Can't read this tag.\n");
                break;
        }
        tag = MB_STRUCT_NEXT_TAG(tag);
    }
}
