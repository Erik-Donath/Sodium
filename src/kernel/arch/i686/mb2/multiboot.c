#include <stddef.h>
#include <kernel/stdio.h>

#include "definition.h"
#include "multiboot.h"

_Static_assert(sizeof(memory_map_entry) == sizeof(mb2_tag_memory_map_entry), "memory_map_entry and mb2_tag_memory_map_entry have to be the same size");

static memory_info mem_info;

bool mb_parse(mb_info_ptr mb) {
    // Reset stored values to 0
    mem_info = (memory_info){
        .mem_lower = 0,
        .mem_upper = 0,
        .entry_count = 0,
        .entries = 0
    };

    mb2_header* header = (mb2_header*)mb;
    mb2_tag* tag = header->tags;
    if(!header || header->reserved)
        return false;
    
    while((uint8_t*)tag < (uint8_t*)header + header->total_size) {
        switch(tag->type) {
            case MB_TAG_BASIC_MEMORY_INFORMATION:
                mb2_tag_basic_meminfo* basic_meminfo = (mb2_tag_basic_meminfo*)tag;
                mem_info.mem_lower = basic_meminfo->mem_lower;
                mem_info.mem_upper = basic_meminfo->mem_upper;
                break;
            case MB_TAG_MEMORY_MAP:
                mb2_tag_memory_map* mem_map = (mb2_tag_memory_map*)tag;
                mem_info.entry_count = (tag->size - sizeof(mb2_tag_memory_map)) / sizeof(mb2_tag_memory_map_entry);
                mem_info.entries = (memory_map_entry*)mem_map->entries;
                break;
            default:
                break;
        }
        tag = MB_STRUCT_NEXT_TAG(tag);
    }
    return true;
}

const memory_info* mb_getMemoryInfo() {
    return &mem_info;
}
