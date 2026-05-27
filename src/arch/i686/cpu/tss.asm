bits 32
section .text

%include "arch/i686/cpu/segments.inc"

global i686_tss_load
i686_tss_load:
    ; no parameters given

    ; Loading TSS via ltr instruction
    mov ax, i686_TSS_SEGMENT
    ltr ax

    ret
