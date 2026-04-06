#pragma once

#include "io.h"
#include <stdint.h>
#define i686_DEBUG_PORT 0xE9

#ifdef SODIUM_DEBUG
  _Static_assert(sizeof(char) == sizeof(uint8_t), "char and uint8_t must be the same size!");
  #define i686_debug_putc(c) i686_io_outb(i686_DEBUG_PORT, (uint8_t)(c))
#else
  #define i686_debug_putc(c) (void)(c)
#endif

static inline void i686_debug_puts(char* str) {
  while(*str) 
    i686_debug_putc(*(str++));
}