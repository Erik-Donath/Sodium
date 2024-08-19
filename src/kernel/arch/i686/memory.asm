bits 32

section .text
global memcpy
memcpy:
    mov edi, [esp+0x4] ; dest
    mov esi, [esp+0x8] ; src
    mov ecx, [esp+0xc] ; size
    rep movsb
    ret

global memset
memset:
    mov edi, [esp+0x4] ; dest
    mov esi, [esp+0x8] ; fill
    mov ecx, [esp+0xc] ; size
    rep stosb
    ret

section .note.GNU-stack noalloc noexec nowrite progbits