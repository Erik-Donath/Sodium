bits 32

section .text

global i686_idt_flush
i686_idt_flush:
    ; [esp + 4] = idt pointer

    ; Loading IDT via lidt instruction
    mov eax, [esp + 4]
    lidt [eax]

    ret