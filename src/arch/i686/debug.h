#pragma once

#include "cpu/io.h"
#include <stdint.h>

// @brief The E9 Port is used to write to the stdio of a emulation engine such as QEMU. On real hardware this has no effect. Please compile in release mode for real hardware to safe io writes!
#define I686_DEBUG_PORT  0xE9

// @brief Print to Emulator stdio if compiled in Debug Mode
// @param c The Char to print to stdio
static inline void i686_debug_putc(char c) {
#ifdef SODIUM_DEBUG
    i686_io_outb(I686_DEBUG_PORT, (uint8_t)c);
#else
    (void)c;
#endif
}
