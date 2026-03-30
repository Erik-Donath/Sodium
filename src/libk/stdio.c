#include "stdio.h"
#include <stdarg.h>

// #FIXME: Temporarly defining putc to output to debug port.
#include "../arch/i686/debug.h"
void putc(char c) { i686_debug_putc(c); }

void printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}
void vprintf(const char *fmt, va_list args) { return; }