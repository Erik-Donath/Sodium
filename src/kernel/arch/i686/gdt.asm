bits 32
section .text

global i686_GDT_Flush
i686_GDT_Flush:
    ; Function parameters:
    ; [esp + 4] = gdt_pointer ptr
    ; [esp + 8] = uint16_t codeSegment
    ; [esp + 12] = uint16_t dataSegment
    push ebp
    mov ebp, esp

    ; load gdt
    mov eax, [ebp + 8]
    lgdt [eax]

    ; reload code segment
    mov eax, [ebp + 12]
    push eax
    push .reload
    retf
.reload:
    ; reload data segment
    mov eax, [esp + 16]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, ebp
    pop ebp
    ret

section .note.GNU-stack noalloc noexec nowrite progbits