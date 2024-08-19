#pragma once
#include <stdint.h>

void ASMCALL memcpy(uint32_t dest, uint32_t src, uint32_t size);
void ASMCALL memset(uint32_t dest, uint32_t fill, uint32_t size);
