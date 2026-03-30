bits 32
section .text

global i686_tss_flush
i686_tss_flush:
    ; [esp + 4] = tss segment

    ; Loading TSS via ltr instruction
    mov ax, [esp + 4]
    ltr ax

    ret

section .note.GNU-stack noalloc noexec nowrite progbits