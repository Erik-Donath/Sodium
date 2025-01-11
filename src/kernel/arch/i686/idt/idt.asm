bits 32
section .text

%include "src/kernel/arch/i686/idt/isr.inc"

extern i686_IDT_Handler
%define GDT_KERNEL_DATA_SEGMENT 0x10

global i686_ISR_common
i686_ISR_common:
    ; Save general-purpose registers
    pusha

    ; Save segment registers
    push ds
    push es
    push fs
    push gs

    ; Load the kernel data segment into the data segment registers
    mov ax, GDT_KERNEL_DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call the C handler function
    push esp
    call i686_IDT_Handler
    add esp, 4

    ; Restore segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore general-purpose registers
    popa

    ; Check stack alignment and clean up
    add esp, 8  ; Remove error code and interrupt number
    iret


global i686_IDT_Flush
i686_IDT_Flush:
    push ebp
    mov ebp, esp
    ; Function parameters:
    ; [ebp + 8]  = idt_pointer* ptr

    ; load idt
    mov eax, [ebp + 8]
    lidt [eax]

    mov esp, ebp
    pop ebp
    ret

section .note.GNU-stack noalloc noexec nowrite progbits