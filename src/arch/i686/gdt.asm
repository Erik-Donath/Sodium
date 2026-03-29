bits 32
section .text

global i686_gdt_flush
i686_gdt_flush:
    ; [ebp + 8] = gdt pointer
    ; [ebp + 12] = code segment
    ; [ebp + 16] = data segment

    ; Setup Stack Frame
    push ebp
    mov ebp, esp

    ; loading the gdt via the lgdt instruction
    mov eax,  [ebp + 8]
    lgdt [eax]

    ; reload code segment
    ; Im using the far return instruction to reload the code segment. Therefor the code segment and return adress need to be pushed on the stack. 
    ; The retf instruction will then pop those values of the stack and perform a far jump and reload the code segment.
    mov eax, [esp + 12]
    push eax
    push .code_reloaded
    retf

.code_reloaded:
    ; reload data segment
    mov eax, [esp + 16]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Stack Frame and Return
    mov esp, ebp
    pop ebp
    ret

section .note.GNU-stack noalloc noexec nowrite progbits