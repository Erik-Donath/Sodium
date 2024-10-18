#pragma once
#include <stdint.h>
#include <kernel/util/assembly.h>

typedef struct {
    // In reversed order they are pushed
    uint32_t ds;                                            // data segment
    uint32_t edi, esi, ebp, kern_esp, ebx, edx, ecx, eax;   // pusha
    uint32_t interrupt, error;                              // interrupt number and error code
    uint32_t eip, cs, eflags, esp, ss;                      // pushed by CPU
} ASMPACK ISR_Registers;

typedef void (*ISRHandler)(ISR_Registers* regs);

void i686_ISR_Initilize();
void i686_ISR_RegisterHandler(uint8_t interrupt, ISRHandler handler);
