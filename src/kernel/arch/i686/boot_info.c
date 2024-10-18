#include <stdint.h>

#include <kernel/util/assembly.h>
#include "boot_info.h"

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
#define MB_STRUCT_NEXT_TAG(tag) (multiboot_tag*)((uint8_t*)tag + MB_STRUCT_ALIGN_UP(tag->size, MB_STRUCT_TAG_ALIGN))

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

/* MULTIBOOT COMMON TAG STRUCTS */

typedef struct {
    multiboot_tag head;
    char string[0];
} ASMPACK multiboot_tag_string;

typedef struct {
    multiboot_tag head;
    uint32_t ptr;
} ASMPACK multiboot_tag_32bit_ptr;

typedef struct {
    multiboot_tag head;
    uint64_t ptr;
} ASMPACK multiboot_tag_64bit_ptr;

typedef multiboot_tag_string multiboot_tag_command_line;
typedef multiboot_tag_string multiboot_tag_bootloader_name;
typedef multiboot_tag_32bit_ptr multiboot_tag_efi_32_system_table_ptr;
typedef multiboot_tag_32bit_ptr multiboot_tag_efi_32_image_handle_ptr;
typedef multiboot_tag_64bit_ptr multiboot_tag_efi_64_system_table_ptr;
typedef multiboot_tag_64bit_ptr multiboot_tag_efi_64_image_handle_ptr;

/* MULTIBOOT TAG STRUCTS*/

typedef struct {
    multiboot_tag head;
    uint32_t mem_lower;
    uint32_t mem_upper;
} ASMPACK multiboot_tag_basic_meminfo;

typedef struct {
    multiboot_tag head;
    uint32_t bios_device;
    uint32_t partition;
    uint32_t sub_partition;
} ASMPACK multiboot_tag_bios_boot_device;

typedef struct {
    multiboot_tag head;
    uint32_t start;
    uint32_t end;
    char string[0];
} ASMPACK multiboot_tag_modules;

typedef struct {
    multiboot_tag head;
    uint16_t num;
    uint16_t entsize;
    uint16_t shndx;
    uint16_t reserved;
    uint8_t  section_headers[0];
    /* FIXME: section_headers must be implemented */
} ASMPACK multiboot_tag_elf_symbols;

typedef struct {
    uint64_t base_addr;
    uint64_t lenght;
    uint32_t type;
    uint32_t reserved;
} ASMPACK multiboot_tag_memory_map_entry;

typedef struct {
    multiboot_tag head;
    uint32_t entry_size;
    uint32_t entry_version;
    multiboot_tag_memory_map_entry entries[0];
} ASMPACK multiboot_tag_memory_map;

typedef struct {
    multiboot_tag head;
    uint16_t version;
    uint16_t creg;
    uint32_t offset;
    uint16_t creg_16;
    uint16_t dseg;
    uint16_t flags;
    uint16_t cseg_len;
    uint16_t cseg_16_len;
    uint16_t dseg_len;
} ASMPACK multiboot_tag_apm_table;

typedef struct {
    multiboot_tag head;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint8_t  vbe_control_info[512];
    uint8_t  vbe_mode_info[256];
    /* FIXME: Print vbe_control_info & vbe_mode_info on console. */
} ASMPACK multiboot_tag_vbe_info;

typedef struct {
    multiboot_tag head;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint8_t  framebuffer_reserved;
    uint8_t  color_info[0];
    /* FIXME: Read Frame buffer color information. */
} ASMPACK multiboot_tag_framebuffer_info;

typedef struct {
    multiboot_tag head;
    uint8_t major;
    uint8_t minor;
    uint8_t reserved[6];
    uint8_t smbios_tables[0];
    /* FIXME: SMBIOS tables */
} ASMPACK multiboot_tag_smbios_tables;

typedef struct {
    multiboot_tag head;
    uint8_t rsdp[0];
    /* FIXME: copy of RSDPv1 */
} ASMPACK multiboot_tag_acpi_old_rsdp;

typedef struct {
    multiboot_tag head;
    uint8_t rsdp[0];
    /* FIXME: copy of RSDPv2 */
} ASMPACK multiboot_tag_acpi_new_rsdp;

typedef struct {
    multiboot_tag head;
    uint8_t dhcp_ack[0];
    /* FIXME: DHCP ACK */
} ASMPACK multiboot_tag_network_info;

typedef struct {
    multiboot_tag head;
    uint32_t descriptor_size;
    uint32_t descriptor_version;
    uint8_t efi_memory_map[0];
    /* FIXME: EFI memory map */
} ASMPACK multiboot_tag_efi_memory_map;

typedef struct {
    multiboot_tag head;
    uint32_t load_base_addr;
} ASMPACK multiboot_tag_image_load_base_physical_address;


// ! INFO Section

static multiboot_header* INFO_multiboot_header = 0x0;

void INFO_ParseMultiboot(void* mb_info) {
    INFO_multiboot_header = (multiboot_header*)mb_info;

    /* PARSE INFO STRUKT*/
    // Last Implementation: https://github.com/Erik-Donath/Sodium/blob/a7be9620ee8b1e8e39183cc0025125de0555acac/src/kernel/sysinfo.c
}

void* INFO_GetMultiboot() {
    return (void*)INFO_multiboot_header;
}

