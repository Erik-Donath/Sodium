#include "heap.h"

#include "../hal.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Internal

#define PAGE_SIZE      4096u
#define INITIAL_PAGES  1u
#define ALIGN          8u
#define MIN_SPLIT      8u
#define MAX_ALLOC_SIZE (PAGE_SIZE - sizeof(k_heap_block_t))

typedef struct k_heap_block {
    struct k_heap_block *next;
    size_t               data_size;
    uint8_t              data[0];
} k_heap_block_t;

static bool          initialised    = false;
static size_t        pages_allocated = 0;
static k_heap_block_t *free_list    = NULL;

// @brief Allocate one physical page and insert it into the free list
//        in sorted (ascending address) order.
//        Prints a warning and returns false if the PMM is out of memory.
static bool k_heap_add_page(void);

// Definitions

static bool k_heap_add_page(void) {
    void *page = hal_phy_page_alloc();
    if (!page) {
        printf("[WARN] Kernel heap failed to allocate another physical page\n");
        return false;
    }

    k_heap_block_t *block = (k_heap_block_t *)page;
    block->data_size = MAX_ALLOC_SIZE;

    // Insert in sorted order so the free list invariant is maintained
    // and k_heap_free's coalescing works correctly.
    k_heap_block_t **prev = &free_list;
    while (*prev && *prev < block)
        prev = &((*prev)->next);
    block->next = *prev;
    *prev = block;

    pages_allocated++;
    return true;
}

void k_heap_init(void) {
    if (initialised) return;
    for (size_t i = 0; i < INITIAL_PAGES; i++)
        if (!k_heap_add_page()) return; // initialised stayes false
    initialised = true;
}

void *k_heap_alloc(size_t size) {
    if (!size || size > MAX_ALLOC_SIZE) return NULL;

    size = (size + ALIGN - 1) & ~(ALIGN - 1);

    k_heap_block_t **best_prev = NULL;
    size_t           best_size = (size_t)-1;

    k_heap_block_t **prev = &free_list;
    while (*prev) {
        if ((*prev)->data_size >= size && (*prev)->data_size < best_size) {
            best_prev = prev;
            best_size = (*prev)->data_size;
        }
        prev = &((*prev)->next);
    }

    if (!best_prev) {
        if (!k_heap_add_page()) return NULL;
        return k_heap_alloc(size);  // Guaranteed recursion depth of 1: size <= MAX_ALLOC_SIZE and page of size MAX_ALLOC_SIZE is added.
    }

    k_heap_block_t *block = *best_prev;
    if ((block->data_size - size) >= (sizeof(k_heap_block_t) + MIN_SPLIT)) {
        k_heap_block_t *rem = (k_heap_block_t *)((uint8_t *)block
                              + sizeof(k_heap_block_t) + size);
        rem->next        = block->next;
        rem->data_size   = block->data_size - size - sizeof(k_heap_block_t);
        *best_prev       = rem;
        block->data_size = size;
    } else {
        *best_prev = block->next;
    }

    block->next = NULL;
    return (void *)block->data;
}

size_t k_heap_alloc_size(void *ptr) {
    if (!ptr) return 0;
    return ((k_heap_block_t *)((uint8_t *)ptr
            - offsetof(k_heap_block_t, data)))->data_size;
}

void k_heap_free(void *ptr) {
    if (!ptr) return;

    k_heap_block_t *block =
        (k_heap_block_t *)((uint8_t *)ptr - offsetof(k_heap_block_t, data));

    // Walk to the sorted insertion point.
    // prev_prev stays one step behind prev so that after a predecessor
    // coalesce we can restore *prev == block without an extra traversal.
    k_heap_block_t **prev_prev = &free_list;
    k_heap_block_t **prev      = &free_list;
    while (*prev && *prev < block) {
        prev_prev = prev;
        prev      = &((*prev)->next);
    }

    if (*prev == block) {
        printf("[WARN] Kernel heap double free detected for block at %p.\n",
               block);
        return;
    }

    block->next = *prev;
    *prev       = block;

    uintptr_t block_page = (uintptr_t)block & ~(uintptr_t)(PAGE_SIZE - 1);

    // Coalesce with successor
    if (block->next) {
        uint8_t  *exp      = (uint8_t *)block + sizeof(k_heap_block_t)
                             + block->data_size;
        uintptr_t next_pg  = (uintptr_t)block->next & ~(uintptr_t)(PAGE_SIZE - 1);
        if ((uint8_t *)block->next == exp && next_pg == block_page) {
            block->data_size += sizeof(k_heap_block_t) + block->next->data_size;
            block->next       = block->next->next;
        }
    }

    // Coalesce with predecessor
    // prev is &(pred->next).
    // After merging, reassign prev to prev_prev so *prev == merged block.
    if (prev != &free_list) {
        k_heap_block_t *pred =(k_heap_block_t *)((uint8_t *)prev - offsetof(k_heap_block_t, next));
        uint8_t *exp = (uint8_t *)pred + sizeof(k_heap_block_t) + pred->data_size;
        uintptr_t pred_pg = (uintptr_t)pred & ~(uintptr_t)(PAGE_SIZE - 1);
        if ((uint8_t *)block == exp && pred_pg == block_page) {
            pred->data_size += sizeof(k_heap_block_t) + block->data_size;
            pred->next       = block->next;
            block            = pred;
            prev             = prev_prev;
        }
    }

    // Return whole page to PMM when a whole page is freed
    if ((uintptr_t)block == block_page && block->data_size == MAX_ALLOC_SIZE && pages_allocated  >  INITIAL_PAGES) {
        *prev = block->next;
        hal_phy_page_free((void *)block_page);
        pages_allocated--;
    }
}
