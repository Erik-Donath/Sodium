bits 32
section .text

%define GDT_KERNEL_DATA_SEGMENT 0x10

%macro ISR_NOERRCODE 1
global i686_ISR_%1
i686_ISR_%1:
    push dword 0                 ; Dummy-Fehlercode
    push dword %1                ; Interrupt-Nummer
    jmp i686_ISR_common
%endmacro

%macro ISR_ERRCODE 1
global i686_ISR_%1
i686_ISR_%1:
    push dword %1                ; Interrupt-Nummer
    jmp i686_ISR_common
%endmacro

; ISRs 0-31 (Processor Exceptions)
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE   30
ISR_NOERRCODE 31

; ISRs 32-255 (User-defined Interrupts)
%assign i 32
%rep 255-31
ISR_NOERRCODE i
%assign i i+1
%endrep

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
    mov eax, esp
    add eax, 20
    push eax
    call i686_ISR_Handler

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

    ; clear interrupts
    cli

    ; load idt
    mov eax, [ebp + 8]
    lidt [eax]

    ; set interrupts
    sti

    mov esp, ebp
    pop ebp
    ret

section .note.GNU-stack noalloc noexec nowrite progbits