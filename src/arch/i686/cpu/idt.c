#include "idt.h"
#include "segments.h"

#include <stdint.h>

// This File works together with the ISR definisions and the PIC. Please refear to those if missing some information.

// See: https://wiki.osdev.org/Interrupt_Descriptor_Table

/*
Interrupt Dectrior Table Pointer:
| Bits    | Description                               |
|---------|-------------------------------------------|
| 0-15    | Table limit                               |
| 16-47   | Pointer to the first Entry                |

Interrupt Decriptor Table Entry:
| Bits    | Description                             |
|---------|-----------------------------------------|
| 0-15    | Offset (lower 16 bits)                  |
| 16-31   | Selector (segment selector)             |
| 32-39   | Reserved (must be zero)                 |
| 40-43   | Type (gate type: task, interrupt, trap) |
| 44-47   | Flags                                   |
| 48-63   | Offset (higher 16 bits)                 |
*/

typedef enum i686_idt_flags : uint8_t {
    // Gate type (bits 3-0)
    IDT_FLAG_GATE_TASK    = 0x05,
    IDT_FLAG_GATE_16INT   = 0x06,
    IDT_FLAG_GATE_16TRAP  = 0x07,
    IDT_FLAG_GATE_32INT   = 0x0E,
    IDT_FLAG_GATE_32TRAP  = 0x0F,
 
    // Descriptor Privilege Level (bits 6-5)
    IDT_FLAG_RING0        = 0x00,
    IDT_FLAG_RING1        = 0x20,
    IDT_FLAG_RING2        = 0x40,
    IDT_FLAG_RING3        = 0x60,
 
    // Present (bit 7): must be 1 for any valid gate
    IDT_FLAG_PRESENT      = 0x80,
} i686_idt_flags_t;

typedef struct i686_idt_entry {
    uint16_t offset_low;
    uint16_t segment;
    uint8_t  reserved;
    uint8_t  flags;
    uint16_t offset_high;
} __attribute__((packed)) i686_idt_entry_t;

typedef struct i686_idt_pointer {
    uint16_t size;
    i686_idt_entry_t*   base;
} __attribute__((packed)) i686_idt_pointer_t;

_Static_assert(sizeof(i686_idt_entry_t) == 8, "i686_idt_entry_t musst be 8 bytes long");
_Static_assert(sizeof(i686_idt_pointer_t) == 6, "i686_idt_pointer_t musst be 6 bytes long");

static i686_idt_entry_t idt[256] = { 0 };

static i686_idt_pointer_t idt_ptr = {
    .size = sizeof(idt) - 1,
    .base = idt,
};

static void i686_idt_set(uint8_t int_num, uint32_t offset, uint16_t segment, uint8_t flags) {
    idt[int_num] = (i686_idt_entry_t) {
        .offset_low  = (uint16_t)(offset & 0xFFFF),
        .offset_high = (uint16_t)(offset >> 16),
        .segment     = segment,
        .reserved    = 0,
        .flags       = flags,
    };
}

extern uintptr_t i686_isr_stub[256];

void i686_idt_init(void) {
    for(int i = 0; i < 256; i++)
        i686_idt_set(i, i686_isr_stub[i], i686_KERNEL_CODE_SEGMENT, (IDT_FLAG_PRESENT | IDT_FLAG_RING0 | IDT_FLAG_GATE_32INT));
}

extern void __attribute__((cdecl)) i686_idt_flush(i686_idt_pointer_t* ptr);

void i686_idt_load(void) {
    i686_idt_flush(&idt_ptr);
}