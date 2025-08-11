#pragma once
#include <stdint.h>
#include "kernel/libc/util.h"

typedef uint32_t error_t;
#define ERROR_OK 0 // 0 = Success

void NORETURN panic(const char* msg);
void NORETURN panic_detailed(const char* msg, const char* file, int line, const char* func);

#define PANIC(msg) panic_detailed(msg, __FILE__, __LINE__, __func__)
