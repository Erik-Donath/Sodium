#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../pic.h"

#define i8259A_PIC1_OFFSET 0x20
#define i8259A_PIC2_OFFSET 0x28

bool i8259A_Check();
void i8259A_Enable();
void i8259A_Disable();
void i8259A_Mask(uint8_t irq);
void i8259A_Unmask( uint8_t irq);

uint16_t i8259A_ReadMask();
uint16_t i8259A_ReadIRQRequestRegister();
uint16_t i8259A_ReadIRQServiceRegister();

void i8259A_SendEOI(uint8_t irq);
