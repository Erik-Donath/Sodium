bits 32

section .text
global GDT_LOAD
GDT_LOAD:
    push ebp
    mov ebp, esp

    ; load gdt
    mov eax, [ebp + 8]
    lgdt [eax]

    ; relaod code segment
    mov eax, [ebp + 12]
    push eax
    push .relaod_cs
    retf
.relaod_cs:
    ; reload data segment
    mov eax, [ebp + 16]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, ebp
    pop ebp
    ret

section .note.GNU-stack noalloc noexec nowrite progbits