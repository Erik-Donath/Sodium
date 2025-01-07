#include <stdint.h>
#include <kernel/stdio.h>

#include "../mb2/multiboot.h"
#include "heap.h"


#define MIN_HEAP_SIZE 0x100000
#define MIN_BLOCK_SIZE 8

typedef struct block {
    size_t size;
    struct block* next;
    uint8_t content[0];
} block_t;

static uint8_t* heap_base = 0;
static size_t heap_size = 0;

static block_t* free_list = 0;

bool i686_memory_init() {
    const memory_info* mem_info = mb_getMemoryInfo();
    bool found_block = false;

    for(uint32_t i = 0; i < mem_info->entry_count; i++) {
        const memory_map_entry entry = mem_info->entries[i];
        if(entry.type == MEMORY_INFO_AVAILABLE && entry.lenght >= MIN_HEAP_SIZE) {
            heap_base = (uint8_t*)(uintptr_t)entry.base_addr;
            heap_size = entry.lenght;
            found_block = true;
            break;
        }
    }

    if(found_block) {
        block_t* block = (block_t*)heap_base;
        block->size = heap_size - sizeof(block_t);
        block->next = 0;
        free_list = block;
    }

    return found_block;
}

void i686_memory_info() {
    size_t free_heap = 0;

    block_t* current = free_list;
    while(current) {
        free_heap += current->size;
        current = current->next;
    }
    size_t used_heap = heap_size - free_heap; // including block desciptors
    printf("Heap Block:\n\tbase = 0x%X\n\tsize = %d\n\tused = %d\n\tfree = %d\n", heap_base, heap_size, used_heap, free_heap);
}

void* i686_memory_malloc(size_t size) {
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

    printf("Allocated Block at 0x%X with size %d\n", (void*)block->content, block->size);
    return (void*)block->content;
}

void i686_memory_free(void* ptr) {
    if(!ptr) return;

    // Find block descriptor
    block_t* block = (block_t*)((uint8_t*)ptr - offsetof(block_t, content));

    if((uint8_t*)block < heap_base || (uint8_t*)block >= heap_base + heap_size) {
        // Attempted to free a block outside the heap boundaries.
        return;
    }

    // insert block sorted in free_list
    block_t** prev = &free_list;
    #define current (*prev)
    while(current && current < block) {
        prev = &(current->next);
    }
    block->next = current;
    current = block;

    // Coalesce adjacent free blocks
    printf("Freeing %X\n", block);

    prev = &free_list;
    while(current && current->next) {
        block_t* next = current->next;

        if((uint8_t*)(current) + current->size + sizeof(block_t) == (uint8_t*)next) {
            printf("\tCoalesce %X, %X\n", current, next);
            current->size += next->size + sizeof(block_t);
            current->next = next->next;
        }
        else {
            prev = &(current->next);
        }
    }

    return;
}
