#include "idt.h"
#include "segments.h"

#include <stdint.h>

// Internal
//
// IDT entry layout:
//  Bits  0-15  Offset low
//  Bits 16-31  Segment selector
//  Bits 32-39  Reserved (zero)
//  Bits 40-47  Type and flags
//  Bits 48-63  Offset high
// See: https://wiki.osdev.org/Interrupt_Descriptor_Table

enum {
    IDT_FLAG_GATE_TASK   = 0x05,
    IDT_FLAG_GATE_16INT  = 0x06,
    IDT_FLAG_GATE_16TRAP = 0x07,
    IDT_FLAG_GATE_32INT  = 0x0E,
    IDT_FLAG_GATE_32TRAP = 0x0F,
    IDT_FLAG_RING0       = 0x00,
    IDT_FLAG_RING1       = 0x20,
    IDT_FLAG_RING2       = 0x40,
    IDT_FLAG_RING3       = 0x60,
    IDT_FLAG_PRESENT     = 0x80,
};

typedef struct i686_idt_entry {
    uint16_t offset_low;
    uint16_t segment;
    uint8_t  reserved;
    uint8_t  flags;
    uint16_t offset_high;
} __attribute__((packed)) i686_idt_entry_t;

typedef struct i686_idt_pointer {
    uint16_t         size;
    i686_idt_entry_t *base;
} __attribute__((packed)) i686_idt_pointer_t;

_Static_assert(sizeof(i686_idt_entry_t)   == 8, "i686_idt_entry_t must be 8 bytes.");
_Static_assert(sizeof(i686_idt_pointer_t) == 6, "i686_idt_pointer_t must be 6 bytes.");

static i686_idt_entry_t  idt[256]  = {0};
static i686_idt_pointer_t idt_ptr = {
    .size = sizeof(idt) - 1,
    .base = idt,
};

extern uintptr_t i686_isr_stub[256];
extern void __attribute__((cdecl)) i686_idt_flush(i686_idt_pointer_t *ptr);

// @brief Write one IDT gate.
// @param int_num  Vector index (0–255).
// @param offset   Handler address.
// @param segment  Code segment selector.
// @param flags    Gate type and privilege bits.
static void i686_idt_set(uint8_t int_num, uint32_t offset,
                          uint16_t segment, uint8_t flags);

// Definitions

static void i686_idt_set(uint8_t int_num, uint32_t offset, uint16_t segment, uint8_t flags) {
    idt[int_num] = (i686_idt_entry_t){
        .offset_low  = (uint16_t)(offset & 0xFFFF),
        .offset_high = (uint16_t)(offset >> 16),
        .segment     = segment,
        .reserved    = 0,
        .flags       = flags,
    };
}

void i686_idt_init(void) {
    for (int i = 0; i < 256; i++)
        i686_idt_set(i, i686_isr_stub[i], 
            I686_KERNEL_CODE_SEGMENT, 
            IDT_FLAG_PRESENT | IDT_FLAG_RING0 | IDT_FLAG_GATE_32INT
        );
}

void i686_idt_load(void) {
    i686_idt_flush(&idt_ptr);
}
