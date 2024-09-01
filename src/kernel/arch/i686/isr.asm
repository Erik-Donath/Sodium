bits 32

section .text
extern ISR_handler

%macro ISR_NOERRORCODE 1
global ISR_%1
ISR_%1:
    push 0 ; error code
    push %1 ; interrupt number
    jmp isr_common
%endmacro

%macro ISR_ERRORCODE 1
global ISR_%1
ISR_%1:
    push %1 ; interrupt number
    jmp isr_common
%endmacro

%include "src/kernel/arch/i686/isr_gen.inc"

isr_common:
    pusha

    xor eax, eax
    mov ax, ds
    push eax

    mov ax, 0x10 ; kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax 

    push esp ; parse pointer which points to pushed information
    call ISR_handler
    add esp, 4

    pop eax ; old data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax 

    popa
    add esp, 8
    iret

section .note.GNU-stack noalloc noexec nowrite progbits