#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

// @brief Write one character to the debug output (hal_debug_putc).
void putc(char c);

// @brief Formatted print to debug output.
// @return Number of characters written.
uint32_t printf(const char *fmt, ...);

// @brief va_list version of printf().
uint32_t vprintf(const char *fmt, va_list *args);

// @brief Print into an unbounded NUL-terminated buffer.
uint32_t sprintf(char *buf, const char *fmt, ...);

// @brief va_list version of sprintf().
uint32_t vsprintf(char *buf, const char *fmt, va_list *args);

// @brief Print into a bounded buffer. Always NUL-terminates if size > 0.
//        Returns the number of characters that would have been written
//        without the limit (excluding NUL), matching the C99 contract.
uint32_t snprintf(char *buf, size_t size, const char *fmt, ...);

// @brief va_list version of snprintf().
uint32_t vsnprintf(char *buf, size_t size, const char *fmt, va_list *args);
