bits 32

section .text

global i686_fpu_init
i686_fpu_init:
    mov eax, cr0
    and eax, ~(1 << 2)  ; Clear EM  (disable emulation)
    or  eax,  (1 << 1)  ; Set   MP  (monitor coprocessor)
    or  eax,  (1 << 5)  ; Set   NE  (native FPU exceptions)
    and eax, ~(1 << 3)  ; Clear TS  (clear task-switched flag)
    mov cr0, eax

    mov eax, cr4
    or  eax, (1 << 9) | (1 << 10) ; Enable OSFXSR and OSXMMEXCPT (enable SSE instructions and exceptions)
    mov cr4, eax

    fninit              ; Reset FPU to clean known state
    ret