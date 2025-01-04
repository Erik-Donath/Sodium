#include <kernel/stdio.h>

#include "definition.h"
#include "multiboot.h"

bool mb_parse(mb_info_ptr mb) {
    mb2_header* header = (mb2_header*)mb;
    mb2_tag* tag = header->tags;
    if(!header || header->reserved)
        return false;
    
    while((uint8_t*)tag < (uint8_t*)header + header->total_size) {
        switch(tag->type) {
            default:
                break;
        }
        tag = MB_STRUCT_NEXT_TAG(tag);
    }

    return true;
}

static const char* mb_info_tag_name_unknown = "UNKNOWN";
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

void mb_print(mb_info_ptr mb) {
    mb2_header* header = (mb2_header*)mb;
    mb2_tag* tag = header->tags;
    if(!header || header->reserved)
        return;

    printf("Multiboot Info:\n\taddress = %p\n\ttotal_size = %u\n", header, header->total_size);
    while((uint8_t*)tag < (uint8_t*)header + header->total_size) {
        const char* tag_name = mb_info_tag_name_unknown;
        if(tag->type < 23)
            tag_name = mb_info_tag_name_table[tag->type];
        
        printf("Entry at %p %s(%u) with %u bytes\n", tag, tag_name, tag->type, tag->size);
        switch(tag->type) {
            case MB_TAG_END_OF_MULTIBOOT_INFO:
                return;
            case MB_TAG_BASIC_MEMORY_INFORMATION:
                mb2_tag_basic_meminfo* meminfo = (mb2_tag_basic_meminfo*)tag;
                printf("\tmem_lower = %u\n\tmem_upper = %u\n", meminfo->mem_lower, meminfo->mem_upper);
                break;
            case MB_TAG_BIOS_BOOT_DEVICE:
                mb2_tag_bios_boot_device* device = (mb2_tag_bios_boot_device*)tag;
                printf("\tbios_device = %u\n\tpartition = %u\n\tsub_partition = %u\n", device->bios_device, device->partition, device->sub_partition);
                break;
            case MB_TAG_BOOT_COMMAND_LINE:
                mb2_tag_command_line* cmd = (mb2_tag_command_line*)tag;
                printf("\tstring = \"%s\"\n", cmd->string);
                break;
            case MB_TAG_MODULES:
                mb2_tag_modules* modules = (mb2_tag_modules*)tag;
                printf("\tstart = %u\n\tend = %u\n\tstring = \"%s\"\n", modules->start, modules->end, modules->string);
                break;
            case MB_TAG_ELF_SYMBOLS:
                mb2_tag_elf_symbols* symbols = (mb2_tag_elf_symbols*)tag;
                printf("\tnum = %u\n\tnentsize = %u\n\tshndx = %u\n\tsection headers size: %u\n", symbols->num, symbols->entsize, symbols->shndx, tag->size - sizeof(mb2_tag_elf_symbols));
                break;
            case MB_TAG_MEMORY_MAP:
                mb2_tag_memory_map* mem_map = (mb2_tag_memory_map*)tag;
                printf("\tentry_size = %u\n\tentry_version = %u\n\tentries:\n", mem_map->entry_size, mem_map->entry_version);

                mb2_tag_memory_map_entry* mem_entry = mem_map->entries;
                while((uint8_t*)mem_entry < (uint8_t*)tag + tag->size) {
                    printf("\t\tTYPE = %u", mem_entry->type);
                    puts(", BASE = 0x");
                    print_hex64(mem_entry->base_addr, false);
                    puts(", LENGHT = ");
                    print_hex64(mem_entry->lenght, false);
                    putc('\n');

                    mem_entry = (mb2_tag_memory_map_entry*)((uint8_t*)mem_entry + mem_map->entry_size);
                }
                break;
            case MB_TAG_BOOTLOADER_NAME:
                mb2_tag_bootloader_name* bootloader = (mb2_tag_bootloader_name*)tag;
                printf("\tstring = \"%s\"\n", bootloader->string);
                break;
            case MB_TAG_APM_TABLE:
                mb2_tag_apm_table* apm = (mb2_tag_apm_table*)tag;
                printf(
                    "\tversion = %u\n\tcreg = %u\n\toffset = %u\n\tcreg_16 = %u\n\tdseg = %u\n\tflags = %u\n\tcseg_len = %u\n\tcseg_16_len = %u\n\tdseg_len = %u\n",
                    apm->version, apm->creg, apm->offset, apm->creg_16, apm->dseg, apm->flags, apm->cseg_len, apm->cseg_16_len, apm->dseg_len
                );
                break;
            case MB_TAG_VBE_INFO:
                mb2_tag_vbe_info* vbe = (mb2_tag_vbe_info*)tag;
                printf(
                    "\tvbe_mode = %u\n\tvbe_interface_seg = %u\n\tvbe_interface_off = %u\n\tvbe_interface_len = %u\n\tvbe_control_info size: %u\n\tvbe_mode_info size: %u\n",
                    vbe->vbe_mode, vbe->vbe_interface_seg, vbe->vbe_interface_off, vbe->vbe_interface_len, 512, 256
                );
                break;
            case MB_TAG_FRAMEBUFFER_INFO:
                mb2_tag_framebuffer_info* framebuffer = (mb2_tag_framebuffer_info*)tag;
                puts("\tframebuffer_addr = 0x");
                print_hex64(framebuffer->framebuffer_addr, false);
                printf(
                    "\n\tframebuffer_pitch = %u\n\tframebuffer_width = %u\n\tframebuffer_height = %u\n\tframebuffer_bpp = %u\n\tframebuffer_type = %u\n\tcolor_info size: %u\n",
                    framebuffer->framebuffer_pitch, framebuffer->framebuffer_width, framebuffer->framebuffer_height, framebuffer->framebuffer_bpp, framebuffer->framebuffer_type, tag->size - sizeof(mb2_tag_framebuffer_info)
                );
                break;
            case MB_TAG_EFI_32_SYSTEM_TABLE_PTR:
                mb2_tag_efi_32_system_table_ptr* efi_system_32 = (mb2_tag_efi_32_system_table_ptr*)tag;
                printf("\tpointer = %p\n", efi_system_32->ptr);
                break;
            case MB_TAG_EFI_64_SYSTEM_TABLE_PTR:
                mb2_tag_efi_64_system_table_ptr* efi_system_64 = (mb2_tag_efi_64_system_table_ptr*)tag;
                puts("\tpointer = 0x");
                print_hex64(efi_system_64->ptr, false);
                putc('\n');
                break;
            case MB_TAG_SMBIOS_TABLES:
                mb2_tag_smbios_tables* smbios = (mb2_tag_smbios_tables*)tag;
                printf("\tmajor = %u\n\tminor = %u\n\tsmbios_tables size: %u", smbios->major, smbios->minor, tag->size - sizeof(mb2_tag_smbios_tables));
                break;
            case MB_TAG_ACPI_OLD_RSDP:
                mb2_tag_acpi_old_rsdp* acpi_old_rsdp = (mb2_tag_acpi_old_rsdp*)tag;
                printf("\tRSDPv1 table size: %u\n", acpi_old_rsdp->head.size - sizeof(mb2_tag_acpi_old_rsdp));
                break;
            case MB_TAG_ACPI_NEW_RSDP:
                mb2_tag_acpi_new_rsdp* acpi_new_rsdp = (mb2_tag_acpi_new_rsdp*)tag;
                printf("\tRSDPv2 table size: %u\n", acpi_new_rsdp->head.size - sizeof(mb2_tag_acpi_new_rsdp));
                break;
            case MB_TAG_NETWORK_INFO:
                mb2_tag_network_info* network = (mb2_tag_network_info*)tag;
                printf("\tDHCP_ACK size: %u\n", network->head.size - sizeof(mb2_tag_network_info));
                break;
            case MB_TAG_EFI_MEMORY_MAP:
                mb2_tag_efi_memory_map* efi_mem_map = (mb2_tag_efi_memory_map*)tag;
                printf(
                    "\tdescriptor_size = %u\n\tdescriptor_version = %u\n\tEFI memory map size: %u",
                     efi_mem_map->descriptor_size, efi_mem_map->descriptor_version, tag->size - sizeof(mb2_tag_efi_memory_map)
                );
                break;
            case MB_TAG_EFI_BOOT_SERVICES_NOT_TERMINATED:
                /* 
                    IMPORTANT: Indication that ExitBootServices() wasn’t called.
                    KERNEL_PANIC ?
                */

                printf("\tExitBootServices wasn't called. (Kernel panic?)\n");
                break;
            case MB_TAG_EFI_32_IMAGE_HANDLE_PTR:
                mb2_tag_efi_32_image_handle_ptr* efi_32_image_handle = (mb2_tag_efi_32_image_handle_ptr*)tag;
                printf("\tpointer = %p", efi_32_image_handle->ptr);
                break;
            case MB_TAG_EFI_64_IMAGE_HANDLE_PTR:
                mb2_tag_efi_64_image_handle_ptr* efi_64_image_handle = (mb2_tag_efi_64_image_handle_ptr*)tag;
                puts("\tpointer = 0x");
                print_hex64(efi_64_image_handle->ptr, false);
                putc('\n');
                break;
            case MB_TAG_IMAGE_LOAD_BASE_PHYSICAL_ADDRESS:
                mb2_tag_image_load_base_physical_address* image_load = (mb2_tag_image_load_base_physical_address*)tag;
                printf("\tload_base_addr = %p\n", image_load->load_base_addr);
                break;
            default:
                puts("\tUnknown Tag\n");
                break;
        }
        tag = MB_STRUCT_NEXT_TAG(tag);
    }
}
