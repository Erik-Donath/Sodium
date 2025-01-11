ENTRY(start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
    . = 0x00100000;
    os_start = .;

    .multiboot_header ALIGN(8) : {
        KEEP(*(.multiboot_header))
    }

    .text ALIGN(4K) : {
        _start = .;
        *(.text)
        *(.rodata)
    }

    .data ALIGN(4K) : {
        *(.data)
    }

    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss)
    }

    . = ALIGN(4K);
    os_end = .;
}