#pragma once

#include <stdint.h>

#define KERNEL_VIRTUAL_BASE     0xC0000000u
#define KERNEL_PDE_FRAME_SIZE   0x00400000u
#define KERNEL_MIRROR_PAGES     4u
#define KERNEL_MIRROR_BYTES     (KERNEL_MIRROR_PAGES * KERNEL_PDE_FRAME_SIZE)

// @brief Convert a physical address to a virtual address in the kernel space.
#define PHYS_TO_VIRT(addr) ((void *)((uintptr_t)(addr) + KERNEL_VIRTUAL_BASE))

// @brief Convert a virtual address in the kernel space to a physical address.
#define VIRT_TO_PHYS(addr) ((void *)((uintptr_t)(addr) - KERNEL_VIRTUAL_BASE))
