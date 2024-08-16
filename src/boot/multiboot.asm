; Multiboot2 Header
; See: 'https://intermezzos.github.io/book/first-edition/multiboot-headers.html'
; See: 'https://wiki.osdev.org/Multiboot#Multiboot_2'

section .multiboot_header
align 4

mb_header_start:
    ; magic number, arichtecture, lenght
    mb_magic_number  dd 0xe85250d6
    mb_architecture  dd 0 ; protected mode i386
    mb_header_lenght dd (mb_header_end - mb_header_start)

    ; checksum
    mb_checksum dd 0x100000000 - (0xe85250d6 + 0 + (mb_header_end - mb_header_start))

    ; end tag
    mb_type  dw 0
    mb_flags dw 0
    mb_size  dd 0
mb_header_end:

section .note.GNU-stack noalloc noexec nowrite progbits
