
#pragma once
#include <stdint.h>
#include "../libc/util.h"

// Error type for kernel functions
typedef uint32_t error_t;
#define ERROR_OK 0 // 0 = Success

// Panic interface for fatal errors
void NORETURN panic(const char* msg);
void NORETURN panic_detailed(const char* msg, const char* file, int line, const char* func);

// Macro for detailed panic with location
#define PANIC(msg) panic_detailed(msg, __FILE__, __LINE__, __func__)
