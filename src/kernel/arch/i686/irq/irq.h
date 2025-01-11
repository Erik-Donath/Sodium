#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <kernel/util.h>

#include "../idt/idt.h"

// PIC1 - Master (IRQs 0-7)
#define INT_TIMER           0    // IRQ 0 - Programmable Interval Timer
#define INT_KEYBOARD        1    // IRQ 1 - Keyboard
#define INT_CASCADE         2    // IRQ 2 - Cascade for PIC2
#define INT_COM2            3    // IRQ 3 - COM2 (Serial Port 2)
#define INT_COM1            4    // IRQ 4 - COM1 (Serial Port 1)
#define INT_LPT2            5    // IRQ 5 - LPT2 (Parallel Port 2) or Sound Card
#define INT_FLOPPY          6    // IRQ 6 - Floppy Disk Controller
#define INT_LPT1            7    // IRQ 7 - LPT1 (Parallel Port 1) or Spurious

// PIC2 - Slave (IRQs 8-15)
#define INT_RTC             8    // IRQ 8 - Real Time Clock
#define INT_ACPI            9    // IRQ 9 - ACPI (Advanced Configuration and Power Interface)
#define INT_AVAILABLE1      10   // IRQ 10 - Available / SCSI / NIC
#define INT_AVAILABLE2      11   // IRQ 11 - Available / SCSI / NIC
#define INT_MOUSE           12   // IRQ 12 - PS/2 Mouse
#define INT_FPU             13   // IRQ 13 - FPU (Floating Point Unit) / Coprocessor
#define INT_PRIMARY_ATA     14   // IRQ 14 - Primary ATA (IDE) Hard Disk
#define INT_SECONDARY_ATA   15   // IRQ 15 - Secondary ATA Hard Disk


typedef void(*IRQHandler)(ISR_Registers* regs);

bool i686_IRQ_Init();

void i686_IRQ_RegisterHandler(uint8_t irq, IRQHandler handler);
void i686_IRQ_ClearHandler(uint8_t irq);

void ASM_CALL i686_IRQ_Enable();
void ASM_CALL i686_IRQ_Disable();
