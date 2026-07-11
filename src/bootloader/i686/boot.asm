bits 32

; Boot code
section .text
extern i686_pre_kernel
extern bss_start
extern bss_end
extern stack_top
global start

MB2_MAGIC    equ 0x36D76289 ; Multiboot2 magic number

start:
    cli

    ; Verify Multiboot2 magic
    cmp eax, MB2_MAGIC
    jne .invalid_magic

    ; Set up 16KB stack
    mov esp, stack_top
    mov ebp, esp

    ; Zero BSS
    mov edi, bss_start
    mov ecx, bss_end
    sub ecx, edi
    xor eax, eax
    rep stosb

    ; Call pre_kernel(mb_info*)
    push ebx
    call i686_pre_kernel

.shutdown:
    ; Halt the system
    ; FIXME: This is a temporary solution. We should implement a proper shutdown sequence.
    cli
    hlt
    jmp .shutdown

.invalid_magic:
    ; Invalid magic number, halt the system
    cli
    hlt
    jmp .invalid_magic
