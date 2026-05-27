#pragma once

#include "mb2/mb2.h"

// @brief Architecture entry point called by boot.asm after stack and BSS setup.
//        Performs full low-kernel initialisation: GDT, TSS, FPU, MB2, PMM,
//        heap, IDT, ISR, PIC, timer, PS/2, VGA — then calls k_main().
//        Returns to boot.asm which executes i686_shutdown on any return.
// @param mb_info  MB2 info struct pointer from GRUB (EBX).
void __attribute__((cdecl)) i686_pre_kernel(i686_mb2_header_t *mb_info);
