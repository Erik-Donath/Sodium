#include "heap.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../hal.h"

#define HEAP_PAGE_SIZE           4096u
#define HEAP_INITIALE_PAGE_COUNT 1u
#define HEAP_ALIGN               8u
#define HEAP_MIN_SPLIT           8u
#define HEAP_MAX_ALLOC_SIZE      (HEAP_PAGE_SIZE - sizeof(k_heap_block_t))

typedef struct k_heap_block {
    struct  k_heap_block* next;
    size_t  data_size;
    uint8_t data[0];
} k_heap_block_t;

static bool initialised = false;
static size_t pages_allocated = 0;
static k_heap_block_t* free_list = NULL;

static bool k_heap_add_page(void) {
    void* page = hal_phy_page_alloc();
    if(!page) {
        printf("[WARN] Kernel heap failed to allocate another physical page\n");
        return false;
    }

    k_heap_block_t* block = (k_heap_block_t*)page;
    block->data_size = HEAP_MAX_ALLOC_SIZE;
    block->next = free_list;
    free_list = block;
    pages_allocated++;
    return true;
}

void k_heap_init() {
    if(initialised) return;

    for(size_t i = 0; i < HEAP_INITIALE_PAGE_COUNT; i++) {
        if(!k_heap_add_page()) {
            return;
        }
    }
    initialised = true;
}

void* k_heap_alloc(size_t size) {
    if(size == 0 || size > HEAP_MAX_ALLOC_SIZE)
        return NULL;

    // Align size to HEAP_ALIGN bytes
    size = (size + HEAP_ALIGN - 1) & ~(HEAP_ALIGN - 1);

    // Best fit search
    k_heap_block_t** best_prev = 0;
    size_t best_size = (size_t)-1; // Max size_t value

    k_heap_block_t** prev = &free_list;
    while(*prev) {
        if((*prev)->data_size >= size && (*prev)->data_size < best_size) {
            best_prev = prev;
            best_size = (*prev)->data_size;
        }
        prev = &((*prev)->next);
    }

    if(!best_prev) {
        if(!k_heap_add_page())
            return NULL;
        return k_heap_alloc(size); // Try again with recursion
    }

    // Split if it makes sense
    k_heap_block_t* block = *best_prev;
    if((block->data_size - size) >= (sizeof(k_heap_block_t) + HEAP_MIN_SPLIT)) {
        k_heap_block_t* remainder = (k_heap_block_t*)((uint8_t*)block + sizeof(k_heap_block_t) + size);
        remainder->next = block->next;
        remainder->data_size = block->data_size - size - sizeof(k_heap_block_t);
        *best_prev = remainder;
        block->data_size = size;
    }
    else *best_prev = block->next;
    
    block->next = NULL;
    return (void*)block->data;
}

void k_heap_free(void* ptr) {
    if(!ptr) return;

    k_heap_block_t* block = (k_heap_block_t*)((uint8_t*)ptr - offsetof(k_heap_block_t, data));

    // #FIXME: There should be checks against freeing outside of a heap page here!

    // Find insertion
    k_heap_block_t** prev = &free_list;
    while(*prev && *prev <= block)
        prev = &((*prev)->next);
    
    if(*prev == block) {
        printf("[WARN] Kernel heap detected double free attempt for block at %p.\n", block);
        return;
    }

    // Insert
    k_heap_block_t** insert_prev = prev;
    block->next = *prev;
    *prev = block;

    uintptr_t block_page = (uintptr_t)block & ~(uintptr_t)(HEAP_PAGE_SIZE - 1);

    // Try to coalesce with next block if adjacent and on same page
    if(block->next) {
        uint8_t* expected_next = (uint8_t*)block + sizeof(k_heap_block_t) + block->data_size;
        uintptr_t next_page = (uintptr_t)block->next & ~(uintptr_t)(HEAP_PAGE_SIZE - 1);

        if((uint8_t*)block->next == expected_next && block_page == next_page) {
            block->data_size += sizeof(k_heap_block_t) + block->next->data_size;
            block->next = block->next->next;
        }
    }

    // Try to coalesce with prev block if adjacent and on same page
    if(insert_prev != &free_list) {
        uint8_t* expected_prev_end = (uint8_t*)*insert_prev + sizeof(k_heap_block_t) + (*insert_prev)->data_size;
        uintptr_t prev_page = (uintptr_t)*insert_prev & ~(uintptr_t)(HEAP_PAGE_SIZE - 1);

        if((uint8_t*)block == expected_prev_end && block_page == prev_page) {
            (*insert_prev)->data_size += sizeof(k_heap_block_t) + block->data_size;
            (*insert_prev)->next = block->next;
            block = *insert_prev;
        }
    }

    // Try to free the block if it is a whole page
    uintptr_t page_base = (uintptr_t)block & ~(uintptr_t)(HEAP_PAGE_SIZE - 1);
    if((uintptr_t)block == page_base && block->data_size == HEAP_MAX_ALLOC_SIZE && pages_allocated > HEAP_INITIALE_PAGE_COUNT) {
        *insert_prev = block->next;
        hal_phy_page_free((void*)page_base);
        pages_allocated--;
    }
}