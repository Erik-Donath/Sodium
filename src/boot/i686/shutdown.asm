bits 32
section .text
    global i686_shutdown

%macro E 2
    mov al, %1
    out 0xE9, al
    mov al, '.'
    out 0xE9, al
    mov al, %2
    out 0xE9, al
    mov al, ' '
    out 0xE9, al
%endmacro

i686_shutdown:
    cli
    E '1', '1'

    ; Disable paging
    mov eax, cr0
    and eax, 0x7FFFFFFF
    mov cr0, eax

    E '1', '2'

    ; Load GDT for Real Mode and jump
    lgdt [.gdt_descriptor]

    E '1', '3'

    ; Update segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    E '1', '4'

    ; Disable protected mode
    mov eax, cr0
    and al, 0xFE  ; Clear PE bit
    mov cr0, eax

    E '2', '0'
    jmp 0x00:.16bit_real

bits 16
.16bit_real:
    E '3', '0'

    ; Set segment registers for Real Mode
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    cli
    ; E '4', '0'
    ; Try APM Shutdown
    ; mov ax, 5300h
    ; xor bx, bx
    ; int 15h
    ; E '4', '1'
    ; jc .try_bios_shutdown

    ; E '4', '2'
    ; mov ax, 5301h
    ; xor bx, bx
    ; int 15h
    ; jc .try_bios_shutdown

.try_bios_shutdown:
    E '5', '0'
    mov ax, 0x5307
    mov bx, 0x0001
    mov cx, 0x0003
    int 0x15

.try_qemu_shutdown:
    E '6', '0'
    ; QEMU-specific shutdown
    mov dx, 0x604
    mov ax, 0x2000
    out dx, ax

.try_keyboard_reset:
    E '7', '0'
    ; Keyboard controller reset
    mov al, 0xFE
    out 0x64, al
    E '7', '1'

    ; Keyboard controller reset
    mov al, 0xFE
    out 0x64, al

    E '7', '2'
    jmp .end

; .try_triple_fault:
;    ; Trigger Triple Fault
;    xor ax, ax
;    mov ds, ax
;    lidt [cs:idt_descriptor]
;    int 3

.end:
    E '9', '0'
    cli
    hlt
    jmp .16bit_real

; idt_descriptor:
;    dw 0
;    dd 0

.gdt_descriptor:
    dw .gdt_end - .gdt_start - 1
    dd .gdt_start

.gdt_start:
    dd 0, 0                       ; Null descriptor
    dd 0x0000FFFF, 0x00CF9A00     ; 32-bit code descriptor
    dd 0x0000FFFF, 0x00CF9200     ; 32-bit data descriptor
.gdt_end:

section .note.GNU-stack noalloc noexec nowrite progbits