bits 32
section .text

global i686_PORTS_outb
i686_PORTS_outb:
    ; Function parameters:
    ; [esp + 4] = port
    ; [esp + 8] = value

    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, al
    ret

global i686_PORTS_inb
i686_PORTS_inb:
    ; Function parameters:
    ; [esp + 4] = port

    mov dx, [esp + 4]
    xor eax, eax
    in al, dx
    ret

global i686_PORTS_outw
i686_PORTS_outw:
    ; Function parameters:
    ; [esp + 4] = port
    ; [esp + 8] = value

    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, ax
    ret

global i686_PORTS_inw
i686_PORTS_inw:
    ; Function parameters:
    ; [esp + 4] = port

    mov dx, [esp + 4]
    xor eax, eax
    in ax, dx
    ret

global i686_PORTS_outl
i686_PORTS_outl:
    ; Function parameters:
    ; [esp + 4] = port
    ; [esp + 8] = value

    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, eax
    ret

global i686_PORTS_inl
i686_PORTS_inl:
    ; Function parameters:
    ; [esp + 4] = port

    mov dx, [esp + 4]
    xor eax, eax
    in eax, dx
    ret

section .note.GNU-stack noalloc noexec nowrite progbits