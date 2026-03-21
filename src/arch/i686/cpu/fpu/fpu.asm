bits 32
section .text

global i686_FPU_Initialize
i686_FPU_Initialize:
    ; FPU initialization
    fninit

    ; Activate FPU
    mov eax, cr0
    and eax, 0xFFFFFFFB ; Bit 2 unset (EM)
    or eax, 0x2         ; Bit 1 set (MP)
    mov cr0, eax

    ; Optional: FXSAVE/FXRSTOR activate (for SSE)
    mov eax, cr4
    or eax, 0x200       ; Bit 9 set (OSFXSR)
    mov cr4, eax

    ret

section .note.GNU-stack noalloc noexec nowrite progbits