bits 32

section .text
    global start
    extern kernel_main

start:
    ; Deaktivate Interrupts
    cli

    ; Creating Stack of 8KB
    mov esp, stack_top
    mov ebp, esp

    ; Check Multiboot-Magic-Number
    cmp eax, 0x36d76289
    jne .error

    ; Jump in the C-Kernel with the first parameter being the mb_info struct pointer (32bit pointer)
    push ebx
    call kernel_main ; Not expected to return
.error:
    cli
    hlt

section .bss
    stack_bottom: ; Reserviere 8 KB für den Stack
    align 16
    resb 8192
    stack_top:

section .note.GNU-stack noalloc noexec nowrite progbits
