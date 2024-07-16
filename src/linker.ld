ENTRY(start)

SECTIONS {
    /* Definiere den Text-Segment, der den Code enthält */
    .text  ALIGN(4K) : {
        KEEP(*(.multiboot_header))
        *(.text)
        *(.rodata)
    }

    /* Definiere den Daten-Segment */
    .data  ALIGN(4K) : {
        *(.data)
    }

    /* Definiere den BSS-Segment */
    .bss  ALIGN(4K) : {
        *(.bss)
        *(COMMON)
    }

    /* Definiere die End-Adresse */
    . = ALIGN(4K);
    end = .;
}
