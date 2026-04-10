#pragma once

#include <stdint.h>

#define i686_i8259A_irq_size 0x08
#define i686_i8259A_irq_master 0x20
#define i686_i8259A_irq_slave (i686_i8259A_irq_master + i686_i8259A_irq_size)

void i686_i8259A_enable(void);
void i686_i8259A_disable(void);
void i686_i8259A_mask(uint8_t pin);
void i686_i8259A_unmask(uint8_t pin);
void i686_i8259A_send_eoi(uint8_t pin);
