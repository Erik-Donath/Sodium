#pragma once

#include "io.h"
#include <stdint.h>
#define i686_DEBUG_PORT 0xE9
static inline void i686_debug_putc(char c) {
  _Static_assert(sizeof(char) == sizeof(uint8_t),
                 "char and uint8_t must be the same size!");
  i686_io_outb(i686_DEBUG_PORT, (uint8_t)c);
}
