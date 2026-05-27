#pragma once

// @brief Enable x87 FPU and SSE; set CR0/CR4 bits and execute FNINIT.
//        Must be called before any floating-point or SSE instruction.
void __attribute__((cdecl)) i686_fpu_init(void);
