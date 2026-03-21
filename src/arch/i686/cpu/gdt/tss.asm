bits 32
section .text

global i686_TSS_Flush
i686_TSS_Flush:
    push ebp
    mov ebp, esp
    ; Function parameter:
    ; [ebp + 8] = uint16_t tssSegment

    ; Load TSS
    mov ax, [ebp + 8]
    ltr ax

    ; Restore stack frame
    mov esp, ebp
    pop ebp
    ret

section .note.GNU-stack noalloc noexec nowrite progbits