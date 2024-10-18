#pragma once

#include "isr.h"

typedef void (*IRQHandler)(ISR_Registers* regs);

void i686_IRQ_Initilize();
void i686_IRQ_RegisterHandler(uint8_t irq, IRQHandler handler);
