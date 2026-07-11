bits 32

; Multiboot2 Header
; Spec: https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html
section .multiboot_header
align 8

MB2_MAGIC    equ  0xE85250D6
MB2_ARCH     equ  0 ; i386 protected mode
MB2_LENGTH   equ  (mb_header_end - mb_header_start)
MB2_CHECKSUM equ -(MB2_MAGIC + MB2_ARCH + MB2_LENGTH)

global mb_header_start
mb_header_start:
    dd MB2_MAGIC
    dd MB2_ARCH
    dd MB2_LENGTH
    dd MB2_CHECKSUM

    ; End tag
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
mb_header_end:
