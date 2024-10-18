#pragma once
#include <stdint.h>

/*
    Official Intel layout

    INT_NUM	    Event Type	        Short Description
    0x00	    Processor Exception	Division by zero
    0x01	    Processor Exception	Single-step interrupt (see trap flag)
    0x02	    Processor Exception	NMI
    0x03	    Processor Exception	Breakpoint (which benefits from the shorter 0xCC encoding of INT 3)
    0x04    	Processor Exception	Overflow
    0x05    	Processor Exception	Bound Range Exceeded
    0x06    	Processor Exception	Invalid Opcode
    0x07	    Processor Exception	Coprocessor not available
    0x08	    Processor Exception	Double Fault
    0x09    	Processor Exception	Coprocessor Segment Overrun (386 or earlier only)
    0x0A	    Processor Exception	Invalid Task State Segment
    0x0B	    Processor Exception	Segment not present
    0x0C	    Processor Exception	Stack Segment Fault
    0x0D	    Processor Exception	General Protection Fault
    0x0E	    Processor Exception	Page Fault
    0x0F	    Processor Exception	reserved
    0x10	    Processor Exception	x87 Floating Point Exception
    0x11	    Processor Exception	Alignment Check
    0x12	    Processor Exception	Machine Check
    0x13	    Processor Exception	SIMD Floating-Point Exception
    0x14	    Processor Exception	Virtualization Exception
    0x15	    Processor Exception	Control Protection Exception (only available with CET)
    0x16-0x1F	Processor Exception	reserved
    0x20-0x27	Hardware Interrupt	IRQ 0-7
    0x70-0x77	Hardware Interrupt	IRQ 8-15
*/

typedef enum {
    IDT_FLAG_GATE_TASK              = 0x5,
    IDT_FLAG_GATE_16BIT_INT         = 0x6,
    IDT_FLAG_GATE_16BIT_TRAP        = 0x7,
    IDT_FLAG_GATE_32BIT_INT         = 0xE,
    IDT_FLAG_GATE_32BIT_TRAP        = 0xF,

    IDT_FLAG_RING0                  = (0 << 5),
    IDT_FLAG_RING1                  = (1 << 5),
    IDT_FLAG_RING2                  = (2 << 5),
    IDT_FLAG_RING3                  = (3 << 5),

    IDT_FLAG_PRESENT                = 0x80,
} IDT_FLAGS;

void i686_IDT_Initilize();
void i686_IDT_SetGate(uint8_t interrupt, void* base, uint16_t segmentDescriptor, uint8_t flags);
void i686_IDT_EnableGate(uint8_t interrupt);
void i686_IDT_DisableGate(uint8_t interrupt);
