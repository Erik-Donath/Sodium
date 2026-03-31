#pragma once
#include <stdarg.h>
#include <stdint.h>

void putc(char c);

uint32_t vprintf(const char *fmt, va_list args);
uint32_t printf(const char *fmt, ...);