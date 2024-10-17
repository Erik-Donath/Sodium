#pragma once

#include "isr.h"

typedef void (*IRQHandler)(ISR_Registers* regs);

void IRQ_Initilize();
void IRQ_RegisterHandler(uint8_t irq, IRQHandler handler);
