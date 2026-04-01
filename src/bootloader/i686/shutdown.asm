bits 32
section .text
    global i686_shutdown

i686_shutdown:
    cli

    ; Method 1: APM
    mov eax, cr0
    and eax, 0x7FFFFFFF
    mov cr0, eax

    ; Jump into 16bit real mode
    lidt [.null_idt]
    lgdt [.gdt_descriptor]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov eax, cr0
    and al, 0xFE
    mov cr0, eax

    jmp 0x00:.real_mode

bits 16
.real_mode:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    cli

    mov ax, 0x5300
    xor bx, bx
    int 0x15
    jc .triple_fault

    mov ax, 0x5301
    xor bx, bx
    int 0x15
    jc .triple_fault

    mov ax, 0x5307
    mov bx, 0x0001
    mov cx, 0x0003
    int 0x15

    ; Method 2: Triple fault
.triple_fault:
    int 0x00

    ; Method 3: Halt forever
.halt:
    cli
    hlt
    jmp .halt

.null_idt:
    dw 0
    dd 0

.gdt_descriptor:
    dw .gdt_end - .gdt_start - 1
    dd .gdt_start

.gdt_start:
    dd 0, 0
    dd 0x0000FFFF, 0x00CF9A00
    dd 0x0000FFFF, 0x00CF9200
.gdt_end:
