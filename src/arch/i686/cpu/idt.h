#pragma once

// @brief Initialise all 256 IDT entries pointing to ISR stubs.
void i686_idt_init(void);

// @brief Load the IDT register (LIDT).
void i686_idt_load(void);
