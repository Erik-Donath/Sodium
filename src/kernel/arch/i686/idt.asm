bits 32

section .text
global i686_IDT_LOAD
i686_IDT_LOAD:
    push ebp
    mov ebp, esp
    
    ; load idt
    mov eax, [ebp + 8]
    lidt [eax]

    mov esp, ebp
    pop ebp
    ret

section .note.GNU-stack noalloc noexec nowrite progbits