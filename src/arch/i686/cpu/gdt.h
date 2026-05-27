#pragma once

// @brief Initialise all six GDT entries:
//        null, kernel code/data, user code/data, TSS.
void i686_gdt_init(void);

// @brief Load the GDT (LGDT) and flush all segment registers.
void i686_gdt_load(void);
