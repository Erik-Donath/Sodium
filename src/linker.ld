ENTRY(start)
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
    . = 0x00100000;

    .text  ALIGN(4K) : {
        KEEP(*(.multiboot_header));
        *(.text);
        *(.rodata);
    }
    .data  ALIGN(4K) : {
        *(.data);
    }
    .bss  ALIGN(4K) : {
        *(.bss);
        *(COMMON);
    }

    . = ALIGN(4K);
    end = .;
}
