#pragma once
#include <stdarg.h>

void putc(char c);

void vprintf(const char *fmt, va_list args);
void printf(const char *fmt, ...);