section .text
global start
bits 32

; # Init Code
start:
    ; Setup Stack
    mov esp, stack_top

    ; Check Multiboot2 and store info struct address for the future
    mov dword [mb_info_struct], ebx
    cmp eax, 0x36D76289
    jne multiboot_failed

    ; Check if cpuid is supported
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21 ; Flip Cpuid bit
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    cmp eax, ecx
    je cpuid_failed

    ; Check if Extended Processer Info is supported
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb extended_prosser_info_failed

    ; Check if Long Mode is supported
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29 ; Test if 'ln' bit is set
    jz long_mode_support_failed

    ; print 'OK'
    mov dword [0xb8000], 0x0f4b0f4f
    hlt ; Halt CPU

; # Error Handling

; Print error messages
multiboot_failed:
    mov esi, multiboot_failed_msg
    jmp handle_error
cpuid_failed:
    mov esi, cpuid_failed_msg
    jmp handle_error 
extended_prosser_info_failed:
    mov esi, extended_prosser_info_failed_msg
    jmp handle_error
long_mode_support_failed:
    mov esi, long_mode_support_msg
    jmp handle_error

; Print error message from esi and the stops execution.
handle_error:
    xor ecx, ecx
    mov ah, 0x04
handle_error_print:
    mov al, byte [esi + ecx]
    test al, al
    jz handle_error_end

    mov word [0xb8000 + ecx * 2], ax
    inc ecx
    jmp handle_error_print
handle_error_end:
    hlt
    jmp $ ; JIC

; Data Sections
section .data
    ; Failed Messages
    multiboot_failed_msg             db 'Failed (1): The Bootloader has to support Multiboot2.', 0
    cpuid_failed_msg                 db 'Failed (2): The CPU must support CPUID.', 0
    extended_prosser_info_failed_msg db 'Failed (3): The CPU must support Extended Processer Info.', 0
    long_mode_support_msg            db 'Failed (4): The CPU must support Long Mode.',0

    ; Multiboot2 Info Struct Address
    mb_info_struct dw 0
section .bss
    align 4096
    stack_bottom:
    resb 4096 * 4
    stack_top:
