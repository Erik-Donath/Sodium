bits 32

section .bss
global stack_top
global stack_bottom

stack_bottom:
    align 16
    resb 16384
stack_top:
