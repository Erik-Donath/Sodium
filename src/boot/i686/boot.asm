bits 32

; Multiboot2 Header
section .multiboot_header
align 8
%include "src/boot/i686/multiboot2.inc"

mb_header_start:
    MB2_HEADER_START MB2_ARCH_I386, (mb_header_end - mb_header_start)
    MB2_INFORMATION_REQUEST_TAG MB2_INFO_REQUEST_MEMORY, MB2_INFO_REQUEST_MEMORY_MAP, MB2_INFO_REQUEST_BOOTDEV, MB2_INFO_REQUEST_CMDLINE, MB2_INFO_REQUEST_MODULES
    MB2_CONSOLE_FLAGS_TAG (MB2_CONSOLE_FLAGS_CONSOLE_REQUIRED | MB2_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED)
    MB2_ENTRY_ADDRESS_TAG start
    MB2_MODULE_ALIGN_TAG
    MB2_END_TAG
mb_header_end:

; Bootcode
section .text
    extern pre_main
    extern i686_shutdown
    global start
start:
    ; Deactivate Interrupts (JIC)
    cli

    ; Check Multiboot-Magic
    cmp eax, MB2_BOOTLOADER_MAGIC
    jne i686_shutdown

    ; Creating Stack of 8KB
    mov esp, stack_top
    mov ebp, esp

    ; Jump to the C-Kernel with the first parameter being the mb_info struct pointer (32bit pointer)
    push ebx
    call pre_main
    jmp i686_shutdown

section .bss
    stack_bottom: ; Reserves 8 KB for Stack
    align 16
    resb 16384
    stack_top:

section .note.GNU-stack noalloc noexec nowrite progbits