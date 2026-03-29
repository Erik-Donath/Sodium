#pragma once
#include <stdint.h>

// Please use with caution:
// Note that it is allways cleaner to use the Function names instead of these
// Macros. Use in quick testing should be the primary usecase here!

#define outb(port, value) i686_io_outb((port), (value))
#define inb(port) (i686_io_inb((port)))

#define outw(port, value) i686_io_outw((port), (value))
#define inw(port) (i686_io_inw((port)))

#define outl(port, value) i686_io_outl((port), (value))
#define inl(port) (i686_io_inl((port)))

static inline void i686_io_outb(uint16_t port, uint8_t value) {
  __asm__ volatile("outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

static inline uint8_t i686_io_inb(uint16_t port) {
  uint8_t value;
  __asm__ volatile("inb %w1, %b0" : "=a"(value) : "Nd"(port) : "memory");
  return value;
}

static inline void i686_io_outw(uint16_t port, uint16_t value) {
  __asm__ volatile("outw %w0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

static inline uint16_t i686_io_inw(uint16_t port) {
  uint16_t value;
  __asm__ volatile("inw %w1, %w0" : "=a"(value) : "Nd"(port) : "memory");
  return value;
}

static inline void i686_io_outl(uint16_t port, uint32_t value) {
  __asm__ volatile("outl %l0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

static inline uint32_t i686_io_inl(uint16_t port) {
  uint32_t value;
  __asm__ volatile("inl %w1, %l0" : "=a"(value) : "Nd"(port) : "memory");
  return value;
}

// QEMU DEBUG PORT
// #FIXME: Move to new location
#define i686_QEMU_DEBUG_PORT 0xE9
static inline void i686_io_debug(char *str) {
  _Static_assert(sizeof(char) == sizeof(uint8_t),
                 "char and uint8_t must be the same lenght!");

  while (*str) {
    i686_io_outb(i686_QEMU_DEBUG_PORT, *(str++));
  }
}