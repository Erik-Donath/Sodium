#include <stdint.h>
#include <kernel/util/assembly.h>
#include <kernel/stdio.h>

#include "idt.h"
#include "isr.h"

#include <kernel/stdio.h>

typedef struct {
    // in reversed order they are pushed
    uint32_t ds;                                            // data segment
    uint32_t edi, esi, ebp, kern_esp, ebx, edx, ecx, eax;   // pusha
    uint32_t interrupt, error;                              // interrupt number and error code
    uint32_t eip, cs, eflags, esp, ss;                      // pushed by CPU
} ASMPACK ISR_Registers;

void ISR_InitializeGates();
void ISR_Initilize() {
    ISR_InitializeGates();
    for(uint16_t i = 0; i < 256; i++)
        IDT_EnableGate((uint8_t)i);
}

void ASMCALL ISR_handler(ISR_Registers* regs) {
    printf("Interrupt %p\n", regs->interrupt);
}
