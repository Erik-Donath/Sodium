#include <kernel/libc/stdint.h>
#include <kernel/core/error.h>
#include <kernel/libc/stdio.h>

#include "../../arch.h"
#include "../../boot/multiboot2/multiboot.h"
#include "heap.h"


#define MIN_HEAP_SIZE 0x100000
#define MIN_BLOCK_SIZE 8

typedef struct block {
    size_t size;
    struct block* next;
    uint8_t content[0];
} block_t;

static uint8_t* lowest_heap_base = 0;
static size_t total_heap_size = 0;

static block_t* free_list = 0;

MEM_ERROR_t i686_memory_init() {
    const memory_info* mem_info = mb_getMemoryInfo();
    block_t* last_block = 0;

    uintptr_t os_start_addr = (uintptr_t)&os_start;
    uintptr_t os_end_addr = (uintptr_t)&os_end;

    for(uint32_t i = 0; i < mem_info->entry_count; i++) {
        const memory_map_entry entry = mem_info->entries[i];

        if(entry.type == MEMORY_INFO_AVAILABLE) {
            uintptr_t entry_base = (uintptr_t)entry.base_addr;
            uintptr_t entry_end = entry_base + entry.lenght;

            uint8_t* base  = (uint8_t*)entry_base;
            size_t size = (size_t)entry.lenght;

            // Skip / Adjust OS region
            if((uintptr_t)base >= os_start_addr && (uintptr_t)base <= os_end_addr) {
                base = (uint8_t*)(os_end_addr + 0x1000); // 4KB after OS
                size = (size_t)(entry_end - (uintptr_t)base);
            }

            if(size >= MIN_HEAP_SIZE) {
                block_t* block = (block_t*)base;
                block->size = size - sizeof(block_t);
                block->next = 0;

                if(last_block) last_block->next = block;
                else free_list = block;
                last_block = block;

                if(lowest_heap_base < base)
                    lowest_heap_base = base;
                total_heap_size += size;
            }
        }
    }

    i686_memory_coalesce_adjacent_blocks();
    return last_block ? MEM_ERROR_OK : MEM_ERROR_NO_BLOCK_FOUND;
}

void i686_memory_info() {
    size_t free_heap = 0;

    block_t* current = free_list;
    while(current) {
        free_heap += current->size;
        current = current->next;
    }
    size_t used_heap = total_heap_size - free_heap; // including block desciptors
    printf("Heap Block:\n\tlowest base = 0x%X\n\tsize = %d\n\tused = %d\n\tfree = %d\n", lowest_heap_base, total_heap_size, used_heap, free_heap);

    printf("Free Block(s):\n");
    current = free_list;
    while(current) {
        printf("\t0x%X with %d free bytes\n", current, current->size);
        current = current->next;
    }
}

void* i686_memory_malloc(size_t size) {
    if (size > SIZE_MAX - sizeof(block_t) + 1)
        return NULL; // Size too large to align safely
    size = (size + sizeof(block_t) - 1) & ~(sizeof(block_t) - 1); // Align size

    block_t** best_fit_prev = 0;
    size_t best_fit_size = (size_t) - 1; // max

    // Find best fit block
    block_t** prev = &free_list;
    #define current (*prev)
    while(current) {
        if(current->size >= size && current->size < best_fit_size) {
            best_fit_prev = prev;
            best_fit_size = current->size;
        }
        prev = &(current->next);
    }

    if(!best_fit_prev) {
        // This is only the case when the heap is full
        return 0;
    }

    block_t* block = *best_fit_prev;
    // Split block ?
    if(block->size - size >= sizeof(block_t) + MIN_BLOCK_SIZE) {
        block_t* free_block = (block_t*)((uint8_t*)block + sizeof(block_t) + size);
        free_block->size = block->size - size - sizeof(block_t);
        free_block->next = block->next;
        *best_fit_prev = free_block;
        block->size = size;
    }
    else {
        *best_fit_prev = block->next;
    }
    return (void*)block->content;
}

void i686_memory_free(void* ptr) {
    if(!ptr) return;

    // Find block descriptor
    block_t* block = (block_t*)((uint8_t*)ptr - offsetof(block_t, content));

    if((uint8_t*)block < lowest_heap_base || (uint8_t*)block >= lowest_heap_base + total_heap_size)
        PANIC("Attempted to free a block outside the heap boundaries");

    if(block->size == 0 || block->size > total_heap_size) {
        PANIC("Attempted to free block with invalid size");
    }

    if((uint8_t*)block + sizeof(block_t) + block->size > lowest_heap_base + total_heap_size) {
        PANIC("Attempted to free block extending beyond heap");
    }

    // TODO: Block Magic validation

    // Find prev block and check if block is not in free list
    block_t** prev = &free_list;
    #define current (*prev)
    while(current && current <= block) {
        if(current == block)
            PANIC("Tried to double free block");
        prev = &(current->next);
    }

    // validate block
    if(prev != &free_list && (uint8_t*)current + current->size + sizeof(block_t) != (uint8_t*)block) {
        PANIC("Tried to free invalid block");
    }

    // insert block sorted in free_list
    block->next = current;
    current = block;

    i686_memory_coalesce_adjacent_blocks();
}

void i686_memory_coalesce_adjacent_blocks() {
    if(!free_list) return;

    block_t** prev = &free_list;
    #define current (*prev)

    while(current && current->next) {
        block_t* next = current->next;

        if((uint8_t*)(current) + current->size + sizeof(block_t) == (uint8_t*)next) {
            current->size += next->size + sizeof(block_t);
            current->next = next->next;
        }
        else {
            prev = &(current->next);
        }
    }
}
