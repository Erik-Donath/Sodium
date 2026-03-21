#include <kernel/libc/stdio.h>

#include "definition.h"
#include "smbios.h"

static const char* smbios_type_name_unknown = "UNKNOWN";
static const char* smbios_type_name_table[] = {
    "BIOS_INFORMATION",              // 0
    "SYSTEM_INFORMATION",            // 1
    "BASEBOARD_INFORMATION",         // 2
    "SYSTEM_ENCLOSURE",              // 3
    "PROCESSOR_INFORMATION",         // 4
    "MEMORY_CONTROLLER_INFORMATION", // 5
    "MEMORY_MODULE_INFORMATION",     // 6
    "CACHE_INFORMATION",             // 7
    "PORT_CONNECTOR_INFORMATION",    // 8
    "SYSTEM_SLOTS",                  // 9
    [16] = "PHYSICAL_MEMORY_ARRAY",  // 16
    [17] = "MEMORY_DEVICE",          // 17
    [18] = "32BIT_MEMORY_ERROR_INFO",// 18
    [19] = "MEMORY_ARRAY_MAPPED_ADDR",// 19
    [20] = "MEMORY_DEVICE_MAPPED_ADDR",// 20
    [127] = "END_OF_TABLE"           // 127
};

static const char* get_smbios_type_name(uint8_t type) {
    if(type < sizeof(smbios_type_name_table)/sizeof(smbios_type_name_table[0]) &&
       smbios_type_name_table[type])
        return smbios_type_name_table[type];
    return smbios_type_name_unknown;
}

// Helper: get string by index from structure's string area
static const char* smbios_get_string(const smbios_tag_t* tag, uint8_t index) {
    if(index == 0)
        return "";
    const char* str = (const char*)tag + tag->length;
    for(uint8_t i = 1; i < index; ++i) {
        while(*str) ++str;
        ++str;
    }
    return str;
}

void smbios_print(smbios_info_ptr smbios, uint32_t size) {
    if(!smbios || size < sizeof(smbios_tag_t))
        return;

    printf("SMBIOS Table at %p, size = %u bytes\n", smbios, size);

    const uint8_t* ptr = (const uint8_t*)smbios;
    const uint8_t* end = ptr + size;

    while(ptr + sizeof(smbios_tag_t) <= end) {
        const smbios_tag_t* tag = (const smbios_tag_t*)ptr;
        if(tag->length < sizeof(smbios_tag_t) || ptr + tag->length > end)
            break;

        const char* type_name = get_smbios_type_name(tag->type);
        printf("Entry at %p %s(%u) with %u bytes\n", tag, type_name, tag->type, tag->length);

        switch(tag->type) {
            case SMBIOS_TYPE_BIOS_INFORMATION: {
                const smbios_bios_information_t* bios = (const smbios_bios_information_t*)tag;
                printf("\tvendor = \"%s\"\n", smbios_get_string(tag, bios->vendor));
                printf("\tbios_version = \"%s\"\n", smbios_get_string(tag, bios->bios_version));
                printf("\tbios_release_date = \"%s\"\n", smbios_get_string(tag, bios->bios_release_date));
                printf("\tbios_rom_size = %u\n", bios->bios_rom_size);
                printf("\tbios_characteristics = 0x%llx\n", (unsigned long long)bios->bios_characteristics);
                break;
            }
            case SMBIOS_TYPE_SYSTEM_INFORMATION: {
                const smbios_system_information_t* sys = (const smbios_system_information_t*)tag;
                printf("\tmanufacturer = \"%s\"\n", smbios_get_string(tag, sys->manufacturer));
                printf("\tproduct_name = \"%s\"\n", smbios_get_string(tag, sys->product_name));
                printf("\tversion = \"%s\"\n", smbios_get_string(tag, sys->version));
                printf("\tserial_number = \"%s\"\n", smbios_get_string(tag, sys->serial_number));
                printf("\tUUID = ");
                for(int i = 0; i < 16; ++i)
                    printf("%02x", sys->uuid[i]);
                printf("\n");
                break;
            }
            case SMBIOS_TYPE_BASEBOARD_INFORMATION: {
                const smbios_baseboard_information_t* bb = (const smbios_baseboard_information_t*)tag;
                printf("\tmanufacturer = \"%s\"\n", smbios_get_string(tag, bb->manufacturer));
                printf("\tproduct = \"%s\"\n", smbios_get_string(tag, bb->product));
                printf("\tversion = \"%s\"\n", smbios_get_string(tag, bb->version));
                printf("\tserial_number = \"%s\"\n", smbios_get_string(tag, bb->serial_number));
                break;
            }
            case SMBIOS_TYPE_PROCESSOR_INFORMATION: {
                const smbios_processor_information_t* proc = (const smbios_processor_information_t*)tag;
                printf("\tsocket_designation = \"%s\"\n", smbios_get_string(tag, proc->socket_designation));
                printf("\tprocessor_type = %u\n", proc->processor_type);
                printf("\tprocessor_family = %u\n", proc->processor_family);
                printf("\tmax_speed = %u\n", proc->max_speed);
                printf("\tcurrent_speed = %u\n", proc->current_speed);
                printf("\tprocessor_version = \"%s\"\n", smbios_get_string(tag, proc->processor_version));
                break;
            }
            case SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY: {
                const smbios_physical_memory_array_t* arr = (const smbios_physical_memory_array_t*)tag;
                printf("\tlocation = %u\n\tuse = %u\n\terror_correction = %u\n\tmax_capacity = %u\n",
                    arr->location, arr->use, arr->error_correction, arr->maximum_capacity);
                break;
            }
            case SMBIOS_TYPE_MEMORY_DEVICE: {
                const smbios_memory_device_t* mem = (const smbios_memory_device_t*)tag;
                printf("\tdevice_locator = \"%s\"\n", smbios_get_string(tag, mem->device_locator));
                printf("\tbank_locator = \"%s\"\n", smbios_get_string(tag, mem->bank_locator));
                printf("\tform_factor = %u\n\tsize = %u\n", mem->form_factor, mem->size);
                printf("\tmanufacturer = \"%s\"\n", smbios_get_string(tag, mem->manufacturer));
                printf("\tserial_number = \"%s\"\n", smbios_get_string(tag, mem->serial_number));
                break;
            }
            case SMBIOS_TYPE_END_OF_TABLE:
                puts("\tEnd of Table\n");
                return;
            default:
                puts("\tUnknown or unhandled structure\n");
                break;
        }

        // Move to the next structure (skip formatted area + unformatted string area)
        const uint8_t* next = ptr + tag->length;
        // Skip string-set (ends with double null)
        while(next + 1 < end && (next[0] != 0 || next[1] != 0))
            ++next;
        if(next + 1 < end)
            next += 2; // skip the double null
        else
            break;
        ptr = next;
    }
}
