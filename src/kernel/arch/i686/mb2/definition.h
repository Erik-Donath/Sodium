#pragma once

#include <stdint.h>

#include <kernel/util.h>

// ! MULTIBOOT2 INFO STRUCT DEFINITIONS
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
#define MB_STRUCT_NEXT_TAG(tag) (mb2_tag*)((uint8_t*)tag + MB_STRUCT_ALIGN_UP(tag->size, MB_STRUCT_TAG_ALIGN))

/* MULTIBOOT INFO HEADER and TAG HEADER */

typedef struct {
    uint32_t type;
    uint32_t size;
} PACKED mb2_tag;

typedef struct {
    uint32_t total_size;
    uint32_t reserved;
    mb2_tag tags[0];
} PACKED mb2_header;

/* MULTIBOOT COMMON TAG STRUCTS */

typedef struct {
    mb2_tag head;
    char string[0];
} PACKED mb2_tag_string;

typedef struct {
    mb2_tag head;
    uint32_t ptr;
} PACKED mb2_tag_32bit_ptr;

typedef struct {
    mb2_tag head;
    uint64_t ptr;
} PACKED mb2_tag_64bit_ptr;

typedef mb2_tag_string mb2_tag_command_line;
typedef mb2_tag_string mb2_tag_bootloader_name;
typedef mb2_tag_32bit_ptr mb2_tag_efi_32_system_table_ptr;
typedef mb2_tag_32bit_ptr mb2_tag_efi_32_image_handle_ptr;
typedef mb2_tag_64bit_ptr mb2_tag_efi_64_system_table_ptr;
typedef mb2_tag_64bit_ptr mb2_tag_efi_64_image_handle_ptr;

/* MULTIBOOT TAG STRUCTS*/

typedef struct {
    mb2_tag head;
    uint32_t mem_lower;
    uint32_t mem_upper;
} PACKED mb2_tag_basic_meminfo;

typedef struct {
    mb2_tag head;
    uint32_t bios_device;
    uint32_t partition;
    uint32_t sub_partition;
} PACKED mb2_tag_bios_boot_device;

typedef struct {
    mb2_tag head;
    uint32_t start;
    uint32_t end;
    char string[0];
} PACKED mb2_tag_modules;

typedef struct {
    mb2_tag head;
    uint16_t num;
    uint16_t entsize;
    uint16_t shndx;
    uint16_t reserved;
    uint8_t  section_headers[0]; // ! refer to the i386 ELF documentation for details as how to read the section header(s)
} PACKED mb2_tag_elf_symbols;

// Memory Types in mb2_tag_memory_map_entry
#define MULTIBOOT_MEMORY_AVAILABLE		        1
#define MULTIBOOT_MEMORY_RESERVED		        2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5

typedef struct {
    uint64_t base_addr;
    uint64_t lenght;
    uint32_t type;
    uint32_t reserved;
} PACKED mb2_tag_memory_map_entry;

typedef struct {
    mb2_tag head;
    uint32_t entry_size;
    uint32_t entry_version;
    mb2_tag_memory_map_entry entries[0];
} PACKED mb2_tag_memory_map;

typedef struct {
    mb2_tag head;
    uint16_t version;
    uint16_t creg;
    uint32_t offset;
    uint16_t creg_16;
    uint16_t dseg;
    uint16_t flags;
    uint16_t cseg_len;
    uint16_t cseg_16_len;
    uint16_t dseg_len;
} PACKED mb2_tag_apm_table;

typedef struct {
    mb2_tag head;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint8_t  vbe_control_info[512];
    uint8_t  vbe_mode_info[256];
} PACKED mb2_tag_vbe_info;

typedef struct {
    uint8_t red_value;
    uint8_t green_value;
    uint8_t blue_value;
} PACKED mb2_tag_framebuffer_info_palette;

typedef union {
    struct {
        uint32_t framebuffer_platte_num_colors;
        mb2_tag_framebuffer_info_palette framebuffer_palette[0];
    } indexed_color;
    struct {
        uint8_t framebuffer_red_field_position;
        uint8_t framebuffer_red_mask_size;
        uint8_t framebuffer_green_field_position;
        uint8_t framebuffer_green_mask_size;
        uint8_t framebuffer_blue_field_position;
        uint8_t framebuffer_blue_mask_size;
    } direct_color;
} PACKED mb2_tag_framebuffer_info_color_info;

typedef struct {
    mb2_tag head;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint8_t  framebuffer_reserved;
    mb2_tag_framebuffer_info_color_info color_info;
} PACKED mb2_tag_framebuffer_info;

typedef struct {
    mb2_tag head;
    uint8_t major;
    uint8_t minor;
    uint8_t reserved[6];
    uint8_t smbios_tables[0];
    // FIXME: Implement SMBIOS table definition(s)
} PACKED mb2_tag_smbios_tables;

typedef struct {
    mb2_tag head;
    uint8_t rsdp[0];
    // FIXME: Impement RSDPv1 definiton(s)
} PACKED mb2_tag_acpi_old_rsdp;

typedef struct {
    mb2_tag head;
    uint8_t rsdp[0];
    // FIXME: Impement RSDPv2 definiton(s)
} PACKED mb2_tag_acpi_new_rsdp;

typedef struct {
    mb2_tag head;
    uint8_t dhcp_ack[0];
    // FIXME: Impement DHCP ACK definiton(s)
} PACKED mb2_tag_network_info;

typedef struct {
    mb2_tag head;
    uint32_t descriptor_size;
    uint32_t descriptor_version;
    uint8_t efi_memory_map[0];
    // FIXME: Impement EFI memory map definiton(s)
} PACKED mb2_tag_efi_memory_map;

typedef struct {
    mb2_tag head;
    // If exsists mb2_tag_efi_memory_map should also exsist
} PACKED mb2_tag_efi_boot_services_not_terminated;

typedef struct {
    mb2_tag head;
    uint32_t load_base_addr;
} PACKED mb2_tag_image_load_base_physical_address;
