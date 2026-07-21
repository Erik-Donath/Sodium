ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)

KERNEL_VIRTUAL_BASE = 0xC0000000;

SECTIONS {
    /* Low Group (VMA == LMA) */

    . = 0x00100000;
    os_start = .;

    /* Multiboot2 header must appear in the first 32 KiB of the binary. */
    .multiboot_header ALIGN(8) : {
        KEEP(*(.multiboot_header))
    }

    .boot ALIGN(4K) : {
        *(.boot)
    }

    /* High Group */
    . += KERNEL_VIRTUAL_BASE;

    .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VIRTUAL_BASE) {
        *(.text)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VIRTUAL_BASE) {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VIRTUAL_BASE) {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VIRTUAL_BASE) {
        bss_start = .;
        *(COMMON)
        *(.bss)

        . = ALIGN(16);
        stack_bottom = .;
        . += 16K;
        stack_top = .;

        bss_end = .;
    }

    . = ALIGN(4K);
    os_end = .;
}
