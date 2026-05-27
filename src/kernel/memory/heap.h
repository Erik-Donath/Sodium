#pragma once

#include <stddef.h>

// @brief Initialise the kernel heap.
//        Safe to call multiple times; subsequent calls are no-ops.
//        Requires hal_phy_page_alloc() to be ready.
//        It is expected that the heap will be initilised in the arch-phase
//        and in the high-kernel-phase another time.
void k_heap_init(void);

// @brief Allocate size bytes from the kernel heap (best-fit policy).
//        size is rounded up to the internal alignment.
//        size must be > 0 and <= HEAP_MAX_ALLOC_SIZE (~4 KiB).
// @return Aligned pointer, or NULL on invalid size or OOM.
void *k_heap_alloc(size_t size);

// @brief Return the usable byte count of an allocation.
//        May be slightly larger than requested due to alignment rounding.
// @param ptr  Pointer from k_heap_alloc(). NULL returns 0.
size_t k_heap_alloc_size(void *ptr);

// @brief Free a block and return it to the sorted free list.
//        Coalesces adjacent free blocks; returns whole pages to the PMM.
//        Detects and logs double-frees without modifying state.
// @param ptr  Pointer from k_heap_alloc(). NULL is a no-op.
void k_heap_free(void *ptr);
