section .text
    global start

start:
    cli
    mov esp, stack_top
    mov ebp, esp

    ; Überprüfe die Multiboot-Magic-Number
    cmp eax, 0x36d76289
    jne .error

    ; print 'OK'
    mov dword [0xb8000], 0x0f4b0f4f

    ; In den C-Kernel springen
    extern kernel_main
    push ebx              ; Multiboot Information Structure Pointer
    call kernel_main
.error:
    cli

    ; print 'ERROR' in Red
    mov dword [0xb8000], 0x04520445
    mov dword [0xb8004], 0x044F0452
    mov dword [0xb8008], 0x00000452

    ; Stop execution
    hlt
    jmp .error

section .bss
    resb 8192             ; Reserviere 8 KB für den Stack
    stack_top:
