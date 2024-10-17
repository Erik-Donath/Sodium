#pragma once
#include <stdint.h>

void PIC_Configure(uint8_t offsetPic1, uint8_t offsetPic2);
void PIC_Disable();

void PIC_Mask(uint8_t irq);
void PIC_Unmask(uint8_t irq);

uint16_t PIC_ReadMask();
uint16_t PIC_ReadIRQRequestRegister();
uint16_t PIC_ReadIRQServicRegister();

void PIC_SendEOI(uint8_t irq);
void PIC_SendSEOI(uint8_t irq);
