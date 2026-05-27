ENTRY(start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
    . = 0x00100000;
    os_start = .;

    /* Multiboot2 header must appear in the first 32 KiB of the binary. */
    .multiboot_header ALIGN(8) : {
        KEEP(*(.multiboot_header))
    }

    .text ALIGN(4K) : {
        *(.text)
    }

    .rodata ALIGN(4K) : {
        *(.rodata)
    }

    .data ALIGN(4K) : {
        *(.data)
    }

    .bss ALIGN(4K) : {
        bss_start = .;
        *(COMMON)
        *(.bss)
        bss_end = .;
    }

    . = ALIGN(4K);
    os_end = .;
}
