bits 32

section .text
global IDT_LOAD
IDT_LOAD:
    push ebp
    mov ebp, esp
    
    ; load idt
    mov eax, [ebp + 8]
    lidt [eax]

    mov esp, ebp
    pop ebp
    ret