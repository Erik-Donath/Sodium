bits 32
section .text

%include "arch/i686/cpu/segments.inc"

global i686_gdt_flush
i686_gdt_flush:
    ; [ebp + 8] = gdt pointer

    ; Setup Stack Frame
    push ebp
    mov ebp, esp

    ; loading the gdt via the lgdt instruction
    mov eax,  [ebp + 8]
    lgdt [eax]

    ; reload code segment
    jmp i686_KERNEL_CODE_SEGMENT:.code_reloaded

.code_reloaded:
    ; reload data segment
    mov eax, i686_KERNEL_DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Stack Frame and Return
    mov esp, ebp
    pop ebp
    ret
