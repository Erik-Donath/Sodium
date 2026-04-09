#pragma once

#include <stdint.h>

void i8259A_enable(void);
void i8259A_disable(void);
void i8259A_send_eoi(uint8_t pin);
