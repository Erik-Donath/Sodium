section .text
    bits 32
    global start
    extern kernel_main

start:
    cli
    mov esp, stack_top
    mov ebp, esp

    ; Überprüfe die Multiboot-Magic-Number
    cmp eax, 0x36d76289
    jne .error

    ; In den C-Kernel springen
    push ebx ; Multiboot Information Structure Pointer
    call kernel_main
.error:
    cli

    ; print 'ERROR'
    mov dword [0xb8000], 0x04520445
    mov dword [0xb8004], 0x044F0452
    mov dword [0xb8008], 0x00000452

    ; Stop execution
    hlt
    jmp .error

section .bss
    resb 8192             ; Reserviere 8 KB für den Stack
    stack_top:
