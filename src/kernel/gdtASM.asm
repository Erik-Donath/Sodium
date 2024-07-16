section .text
    bits 32
    global GDT_Load
    
GDT_Load:
    ; create Stack frame
    push ebp
    mov ebp, esp
    
    ; load gdt
    mov eax, [ebp + 8]
    lgdt [eax]

    ; reload code segment
    mov eax, [ebp + 12]
    push eax
    push .reload_cs
    retf

.reload_cs:
    ; reload data segments
    mov ax, [ebp + 16]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax,
    mov ss, ax

    ; restore call frame
    mov esp, ebp
    pop ebp
    ret
