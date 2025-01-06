#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "../pic.h"

// PIC1 - Master (IRQs 0-7)
#define INT_TIMER           0x20    // IRQ 0 - Programmable Interval Timer
#define INT_KEYBOARD        0x21    // IRQ 1 - Keyboard
#define INT_CASCADE         0x22    // IRQ 2 - Cascade for PIC2
#define INT_COM2            0x23    // IRQ 3 - COM2 (Serial Port 2)
#define INT_COM1            0x24    // IRQ 4 - COM1 (Serial Port 1)
#define INT_LPT2            0x25    // IRQ 5 - LPT2 (Parallel Port 2) or Sound Card
#define INT_FLOPPY          0x26    // IRQ 6 - Floppy Disk Controller
#define INT_LPT1            0x27    // IRQ 7 - LPT1 (Parallel Port 1) or Spurious

// PIC2 - Slave (IRQs 8-15)
#define INT_RTC             0x28    // IRQ 8 - Real Time Clock
#define INT_ACPI            0x29    // IRQ 9 - ACPI (Advanced Configuration and Power Interface)
#define INT_AVAILABLE1      0x2A    // IRQ 10 - Available / SCSI / NIC
#define INT_AVAILABLE2      0x2B    // IRQ 11 - Available / SCSI / NIC
#define INT_MOUSE           0x2C    // IRQ 12 - PS/2 Mouse
#define INT_FPU             0x2D    // IRQ 13 - FPU (Floating Point Unit) / Coprocessor
#define INT_PRIMARY_ATA     0x2E    // IRQ 14 - Primary ATA (IDE) Hard Disk
#define INT_SECONDARY_ATA   0x2F    // IRQ 15 - Secondary ATA Hard Disk

bool i8259A_Check();
void i8259A_Enable();
void i8259A_Disable();
void i8259A_Mask(uint8_t irq);
void i8259A_Unmask( uint8_t irq);

uint16_t i8259A_ReadMask();
uint16_t i8259A_ReadIRQRequestRegister();
uint16_t i8259A_ReadIRQServiceRegister();

// It is requirement to use i8259A_SendSEOI
void i8259A_SendEOI(uint8_t irq);
void i8259A_SendSEOI(uint8_t irq);
