bits 32

section .text
global outb
outb:
    mov dx, [esp + 4] ; port
    mov al, [esp + 8] ; value
    out dx, al
    ret

global inb
inb:
    mov dx, [esp + 4] ; port
    xor eax, eax      ; Clear EAX
    in al, dx
    ret

global EnableInterrupts
EnableInterrupts:
    sti
    ret

global DisableInterrupts
DisableInterrupts:
    cli
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
