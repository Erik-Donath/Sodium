bits 32
section .text
    global i686_shutdown

i686_shutdown:
    cli

    ; Disable paging
    mov eax, cr0
    and eax, 0x7FFFFFFF
    mov cr0, eax

    ; Load GDT for Real Mode and jump
    lgdt [.gdt_descriptor]

    ; Update segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Disable protected mode
    mov eax, cr0
    and al, 0xFE  ; Clear PE bit
    mov cr0, eax

    jmp 0x00:.16bit_real

bits 16
.16bit_real:
    ; Set segment registers for Real Mode
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    cli

    ; Try APM Shutdown
    ; mov ax, 5300h
    ; xor bx, bx
    ; int 15h
    ; jc .try_bios_shutdown

    ; mov ax, 5301h
    ; xor bx, bx
    ; int 15h
    ; jc .try_bios_shutdown

.try_bios_shutdown:
    mov ax, 0x5307
    mov bx, 0x0001
    mov cx, 0x0003
    int 0x15

.try_qemu_shutdown:
    ; QEMU-specific shutdown
    mov dx, 0x604
    mov ax, 0x2000
    out dx, ax

.try_keyboard_reset:
    ; Keyboard controller reset
    mov al, 0xFE
    out 0x64, al

    ; Keyboard controller reset
    mov al, 0xFE
    out 0x64, al

    jmp .end

; .try_triple_fault:
;    ; Trigger Triple Fault
;    xor ax, ax
;    mov ds, ax
;    lidt [cs:idt_descriptor]
;    int 3

.end:
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