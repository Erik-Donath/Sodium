bits 32
section .text

global i686_IRQ_Enable
i686_IRQ_Enable:
    ; Function parameters:
    ; None
    sti
    ret

global i686_IRQ_Disable
i686_IRQ_Disable:
    ; Function parameters:
    ; None
    cli
    ret

section .note.GNU-stack noalloc noexec nowrite progbits