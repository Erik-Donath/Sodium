#include "io.h"

// QEMU DEBUG PORT
// #FIXME: Move to new location
#define i686_QEMU_DEBUG_PORT 0xE9
void i686_io_debug(char *str) {
  _Static_assert(sizeof(char) == sizeof(uint8_t),
                 "char and uint8_t must be the same lenght!");

  while (*str) {
    i686_io_outb(i686_QEMU_DEBUG_PORT, *(str++));
  }
}

void i686_io_debug_hex(uint32_t value) {
  // Print "0x" prefix
  i686_io_outb(i686_QEMU_DEBUG_PORT, '0');
  i686_io_outb(i686_QEMU_DEBUG_PORT, 'x');

  // Print 8 hex digits (32-bit value)
  for (int shift = 28; shift >= 0; shift -= 4) {
    uint8_t nibble = (uint8_t)((value >> shift) & 0xFU);
    char hex_char;
    if (nibble < 10) {
      hex_char = '0' + nibble;
    } else {
      hex_char = 'A' + (nibble - 10);
    }
    i686_io_outb(i686_QEMU_DEBUG_PORT, (uint8_t)hex_char);
  }
}