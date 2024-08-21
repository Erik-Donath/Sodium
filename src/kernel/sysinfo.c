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

// Memory Types in multiboot_tag_memory_map_entry
#define MULTIBOOT_MEMORY_AVAILABLE		        1
#define MULTIBOOT_MEMORY_RESERVED		        2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5

/* Not needed */
#define MB_TAG_UNKNOWN "UNKNOWN"
static const char* mb_info_tag_name_table[23] = {
    "END_OF_MULTIBOOT_INFO",
    "BOOT_COMMAND_LINE",
    "BOOTLOADER_NAME",
    "MODULES",
    "BASIC_MEMORY_INFORMATION",
    "BIOS_BOOT_DEVICE",
    "MEMORY_MAP",
    "VBE_INFO",
    "FRAMEBUFFER_INFO",
    "ELF_SYMBOLS",
    "APM_TABLE",
    "EFI_32_SYSTEM_TABLE_PTR",
    "EFI_64_SYSTEM_TABLE_PTR",
    "SMBIOS_TABLES",
    "ACPI_OLD_RSDP",
    "ACPI_NEW_RSDP",
    "NETWORK_INFO",
    "EFI_MEMORY_MAP",
    "EFI_BOOT_SERVICES_NOT_TERMINATED",
    "EFI_32_IMAGE_HANDLE_PTR",
    "EFI_64_IMAGE_HANDLE_PTR",
    "IMAGE_LOAD_BASE_PHYSICAL_ADDRESS"
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

/* MULTIBOOT COMMON TAG STRUCTS */

typedef struct {
    multiboot_tag head;
    char string[0];
} ASMPACK multiboot_tag_string;

typedef multiboot_tag_string multiboot_tag_command_line;
typedef multiboot_tag_string multiboot_tag_bootloader_name;

typedef struct {
    multiboot_tag head;
    uint32_t ptr;
} ASMPACK multiboot_tag_32bit_ptr;

typedef multiboot_tag_32bit_ptr multiboot_tag_efi_32_system_table_ptr;
typedef multiboot_tag_32bit_ptr multiboot_tag_efi_32_image_handle_ptr;

typedef struct {
    multiboot_tag head;
    uint64_t ptr;
} ASMPACK multiboot_tag_64bit_ptr;

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


static multiboot_header* SYSINFO_multiboot_header = 0x0;

void SYSINFO_parse_multiboot(void* mb_info) {
    SYSINFO_multiboot_header = (multiboot_header*)mb_info;
}

void SYSINFO_print_multiboot_info() {
    multiboot_header* header = SYSINFO_multiboot_header;
    puts("Multiboot Info\n");

    if(!header) {
        puts("\tError: multiboot header not set.\n");
        return;
    }
    if(header->reserved) {
        printf("\tError: Reserved value is not zero.\n\treserved_value = %u\n", header->reserved);
        return;
    }

    printf("\taddress = %p\n\ttotal_size = %u\n", header, header->total_size);

    multiboot_tag* tag = header->tags;
    while((uint8_t*)tag < (uint8_t*)header + header->total_size) {
        const char* tag_name = MB_TAG_UNKNOWN;
        if(tag->type < 23)
            tag_name = mb_info_tag_name_table[tag->type];
        
        printf("Entry at %p %s(%u) with %u bytes\n", tag, tag_name, tag->type, tag->size);
        switch(tag->type) {
            case MB_TAG_END_OF_MULTIBOOT_INFO:
                return;
            case MB_TAG_BASIC_MEMORY_INFORMATION:
                multiboot_tag_basic_meminfo* meminfo = (multiboot_tag_basic_meminfo*)tag;
                printf("\tmem_lower = %u\n\tmem_upper = %u\n", meminfo->mem_lower, meminfo->mem_upper);
                break;
            case MB_TAG_BIOS_BOOT_DEVICE:
                multiboot_tag_bios_boot_device* device = (multiboot_tag_bios_boot_device*)tag;
                printf("\tbios_device = %u\n\tpartition = %u\n\tsub_partition = %u\n", device->bios_device, device->partition, device->sub_partition);
                break;
            case MB_TAG_BOOT_COMMAND_LINE:
                multiboot_tag_command_line* cmd = (multiboot_tag_command_line*)tag;
                printf("\tstring = \"%s\"\n", cmd->string);
                break;
            case MB_TAG_MODULES:
                multiboot_tag_modules* modules = (multiboot_tag_modules*)tag;
                printf("\tstart = %u\n\tend = %u\n\tstring = \"%s\"\n", modules->start, modules->end, modules->string);
                break;
            case MB_TAG_ELF_SYMBOLS:
                multiboot_tag_elf_symbols* symbols = (multiboot_tag_elf_symbols*)tag;
                printf("\tnum = %u\n\tnentsize = %u\n\tshndx = %u\n\tsection headers size: %u\n", symbols->num, symbols->entsize, symbols->shndx, tag->size - sizeof(multiboot_tag_elf_symbols));
                break;
            case MB_TAG_MEMORY_MAP:
                multiboot_tag_memory_map* mem_map = (multiboot_tag_memory_map*)tag;
                printf("\tentry_size = %u\n\tentry_version = %u\n\tentries:\n", mem_map->entry_size, mem_map->entry_version);

                multiboot_tag_memory_map_entry* mem_entry = mem_map->entries;
                while((uint8_t*)mem_entry < (uint8_t*)tag + tag->size) {
                    printf("\t\tTYPE = %u", mem_entry->type);
                    puts(", BASE = 0x");
                    print_hex64(mem_entry->base_addr, false);
                    puts(", LENGHT = ");
                    print_hex64(mem_entry->lenght, false);
                    putc('\n');

                    mem_entry = (multiboot_tag_memory_map_entry*)((uint8_t*)mem_entry + mem_map->entry_size);
                }
                break;
            case MB_TAG_BOOTLOADER_NAME:
                multiboot_tag_bootloader_name* bootloader = (multiboot_tag_bootloader_name*)tag;
                printf("\tstring = \"%s\"\n", bootloader->string);
                break;
            case MB_TAG_APM_TABLE:
                multiboot_tag_apm_table* apm = (multiboot_tag_apm_table*)tag;
                printf(
                    "\tversion = %u\n\tcreg = %u\n\toffset = %u\n\tcreg_16 = %u\n\tdseg = %u\n\tflags = %u\n\tcseg_len = %u\n\tcseg_16_len = %u\n\tdseg_len = %u\n",
                    apm->version, apm->creg, apm->offset, apm->creg_16, apm->dseg, apm->flags, apm->cseg_len, apm->cseg_16_len, apm->dseg_len
                );
                break;
            case MB_TAG_VBE_INFO:
                multiboot_tag_vbe_info* vbe = (multiboot_tag_vbe_info*)tag;
                printf(
                    "\tvbe_mode = %u\n\tvbe_interface_seg = %u\n\tvbe_interface_off = %u\n\tvbe_interface_len = %u\n\tvbe_control_info size: %u\n\tvbe_mode_info size: %u\n",
                    vbe->vbe_mode, vbe->vbe_interface_seg, vbe->vbe_interface_off, vbe->vbe_interface_len, 512, 256
                );
                break;
            case MB_TAG_FRAMEBUFFER_INFO:
                multiboot_tag_framebuffer_info* framebuffer = (multiboot_tag_framebuffer_info*)tag;
                puts("\tframebuffer_addr = 0x");
                print_hex64(framebuffer->framebuffer_addr, false);
                printf(
                    "\n\tframebuffer_pitch = %u\n\tframebuffer_width = %u\n\tframebuffer_height = %u\n\tframebuffer_bpp = %u\n\tframebuffer_type = %u\n\tcolor_info size: %u\n",
                    framebuffer->framebuffer_pitch, framebuffer->framebuffer_width, framebuffer->framebuffer_height, framebuffer->framebuffer_bpp, framebuffer->framebuffer_type, tag->size - sizeof(multiboot_tag_framebuffer_info)
                );
                break;
            case MB_TAG_EFI_32_SYSTEM_TABLE_PTR:
                multiboot_tag_efi_32_system_table_ptr* efi_system_32 = (multiboot_tag_efi_32_system_table_ptr*)tag;
                printf("\tpointer = %p\n", efi_system_32->ptr);
                break;
            case MB_TAG_EFI_64_SYSTEM_TABLE_PTR:
                multiboot_tag_efi_64_system_table_ptr* efi_system_64 = (multiboot_tag_efi_64_system_table_ptr*)tag;
                puts("\tpointer = 0x");
                print_hex64(efi_system_64->ptr, false);
                putc('\n');
                break;
            case MB_TAG_SMBIOS_TABLES:
                multiboot_tag_smbios_tables* smbios = (multiboot_tag_smbios_tables*)tag;
                printf("\tmajor = %u\n\tminor = %u\n\tsmbios_tables size: %u", smbios->major, smbios->minor, tag->size - sizeof(multiboot_tag_smbios_tables));
                break;
            case MB_TAG_ACPI_OLD_RSDP:
                multiboot_tag_acpi_old_rsdp* acpi_old_rsdp = (multiboot_tag_acpi_old_rsdp*)tag;
                printf("\tRSDPv1 table size: %u\n", acpi_old_rsdp->head.size - sizeof(multiboot_tag_acpi_old_rsdp));
                break;
            case MB_TAG_ACPI_NEW_RSDP:
                multiboot_tag_acpi_new_rsdp* acpi_new_rsdp = (multiboot_tag_acpi_new_rsdp*)tag;
                printf("\tRSDPv2 table size: %u\n", acpi_new_rsdp->head.size - sizeof(multiboot_tag_acpi_new_rsdp));
                break;
            case MB_TAG_NETWORK_INFO:
                multiboot_tag_network_info* network = (multiboot_tag_network_info*)tag;
                printf("\tDHCP_ACK size: %u\n", network->head.size - sizeof(multiboot_tag_network_info));
                break;
            case MB_TAG_EFI_MEMORY_MAP:
                multiboot_tag_efi_memory_map* efi_mem_map = (multiboot_tag_efi_memory_map*)tag;
                printf(
                    "\tdescriptor_size = %u\n\tdescriptor_version = %u\n\tEFI memory map size: %u",
                     efi_mem_map->descriptor_size, efi_mem_map->descriptor_version, tag->size - sizeof(multiboot_tag_efi_memory_map)
                );
                break;
            case MB_TAG_EFI_BOOT_SERVICES_NOT_TERMINATED:
                /* 
                    IMPORTANT: Indication that ExitBootServices wasn’t called.
                    KERNEL_PANIC ?
                */

                printf("\tExitBootServices wasn't called. (Kernel panic?)\n");
                break;
            case MB_TAG_EFI_32_IMAGE_HANDLE_PTR:
                multiboot_tag_efi_32_image_handle_ptr* efi_32_image_handle = (multiboot_tag_efi_32_image_handle_ptr*)tag;
                printf("\tpointer = %p", efi_32_image_handle->ptr);
                break;
            case MB_TAG_EFI_64_IMAGE_HANDLE_PTR:
                multiboot_tag_efi_64_image_handle_ptr* efi_64_image_handle = (multiboot_tag_efi_64_image_handle_ptr*)tag;
                puts("\tpointer = 0x");
                print_hex64(efi_64_image_handle->ptr, false);
                putc('\n');
                break;
            case MB_TAG_IMAGE_LOAD_BASE_PHYSICAL_ADDRESS:
                multiboot_tag_image_load_base_physical_address* image_load = (multiboot_tag_image_load_base_physical_address*)tag;
                printf("\tload_base_addr = %p\n", image_load->load_base_addr);
                break;
            default:
                puts("\tUnknown Tag\n");
                break;
        }
        tag = MB_STRUCT_NEXT_TAG(tag);
    }
}
