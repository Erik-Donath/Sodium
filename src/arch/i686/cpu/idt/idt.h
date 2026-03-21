#pragma once

#include <stdint.h>

typedef enum {
    // Gate Types
    IDT_FLAG_GATE_TASK              = 0x5,  // Task gate
    IDT_FLAG_GATE_16BIT_INT         = 0x6,  // 16-bit interrupt gate
    IDT_FLAG_GATE_16BIT_TRAP        = 0x7,  // 16-bit trap gate
    IDT_FLAG_GATE_32BIT_INT         = 0xE,  // 32-bit interrupt gate
    IDT_FLAG_GATE_32BIT_TRAP        = 0xF,  // 32-bit trap gate

    // Descriptor Privilege Levels (DPL)
    IDT_FLAG_RING0                  = 0x00,  // Ring 0 (kernel)
    IDT_FLAG_RING1                  = 0x20,  // Ring 1
    IDT_FLAG_RING2                  = 0x40,  // Ring 2
    IDT_FLAG_RING3                  = 0x60,  // Ring 3 (user)

    // Present Bit
    IDT_FLAG_PRESENT                = 0x80   // Segment present
} IDT_FLAGS;

typedef struct {
    // In the order they are pushed
    uint32_t gs, fs, es, ds;                                // segment registers pushed manually
    uint32_t edi, esi, ebp, kernel_esp, ebx, edx, ecx, eax; // pusha
    uint32_t interrupt, error;                              // interrupt number and error code pushed manually
    uint32_t eip, cs, eflags, esp, ss;                      // pushed by CPU
} __attribute__((packed)) ISR_Registers;

#define DEFAULT_GATE (IDT_FLAG_PRESENT | IDT_FLAG_RING0 | IDT_FLAG_GATE_32BIT_INT)
typedef void (*ISRHandler)(ISR_Registers* regs);

void i686_IDT_Initialize();
void i686_IDT_RegisterHandler(uint8_t interrupt, ISRHandler handler);
void i686_IDT_ClearHandler(uint8_t interrupt);
