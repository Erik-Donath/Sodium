bits 32

section .text
global i686_panic
i686_panic:
    cli
    hlt
    jmp i686_panic

section .note.GNU-stack noalloc noexec nowrite progbits