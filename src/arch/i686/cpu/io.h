#pragma once

#include <stdint.h>

// Convenience shorthand macros.
// Prefer the full function names in production code; these exist for testing.
#define outb(port, value)  i686_io_outb((port), (value))
#define inb(port)          (i686_io_inb((port)))
#define outw(port, value)  i686_io_outw((port), (value))
#define inw(port)          (i686_io_inw((port)))
#define outl(port, value)  i686_io_outl((port), (value))
#define inl(port)          (i686_io_inl((port)))

// @brief Disable maskable interrupts (CLI).
static inline void i686_io_disable_interrupts(void) {
    __asm__ volatile("cli" : : :);
}

// @brief Enable maskable interrupts (STI).
static inline void i686_io_enable_interrupts(void) {
    __asm__ volatile("sti" : : :);
}

// @brief Write 8-bit value to I/O port.
static inline void i686_io_outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

// @brief Read 8-bit value from I/O port.
static inline uint8_t i686_io_inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile("inb %w1, %b0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}

// @brief Write 16-bit value to I/O port.
static inline void i686_io_outw(uint16_t port, uint16_t value) {
    __asm__ volatile("outw %w0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

// @brief Read 16-bit value from I/O port.
static inline uint16_t i686_io_inw(uint16_t port) {
    uint16_t value;
    __asm__ volatile("inw %w1, %w0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}

// @brief Write 32-bit value to I/O port.
static inline void i686_io_outl(uint16_t port, uint32_t value) {
    __asm__ volatile("outl %l0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

// @brief Read 32-bit value from I/O port.
static inline uint32_t i686_io_inl(uint16_t port) {
    uint32_t value;
    __asm__ volatile("inl %w1, %l0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}

// Port 0x80 is the POST diagnostic port. The BIOS disables it before handing
// off, so writes are harmless and take ~1-4 µs — enough to satisfy legacy
// chip timing requirements.
#define I686_IO_DELAY_PORT  0x80

// @brief Burn ~1-4 µs by writing to the POST diagnostic port.
//        Used to satisfy timing requirements when programming PIC, PIT, PS/2.
static inline void i686_io_wait(void) {
    i686_io_outb(I686_IO_DELAY_PORT, 0x00);
}
