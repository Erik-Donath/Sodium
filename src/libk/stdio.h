#pragma once
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

void putc(char c);

uint32_t printf(const char *fmt, ...);
uint32_t vprintf(const char *fmt, va_list* args);
uint32_t sprintf(char *buf, const char *fmt, ...);
uint32_t vsprintf(char *buf, const char *fmt, va_list *args);
uint32_t snprintf(char *buf, size_t size, const char *fmt, ...);
uint32_t vsnprintf(char *buf, size_t size, const char *fmt, va_list *args);