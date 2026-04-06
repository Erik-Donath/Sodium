#include "idt.h"
#include "segments.h"

#include <stdint.h>

/*
Instruction Dectrior Table Pointer:
| Bits    | Description                               |
|---------|-------------------------------------------|
| 0-15    | Table limit                               |
| 16-47   | Pointer to the first Entry                |

Instruction Decriptor Table Entry:
| Bits    | Description                             |
|---------|-----------------------------------------|
| 0-15    | Offset (lower 16 bits)                  |
| 16-31   | Selector (segment selector)             |
| 32-39   | Reserved (must be zero)                 |
| 40-43   | Type (gate type: task, interrupt, trap) |
| 44      | Storage Segment (always 0 for IDT)      |
| 45-46   | Descriptor Privilege Level (DPL)        |
| 47      | Present flag (P)                        |
| 48-63   | Offset (higher 16 bits)                 |
*/

typedef struct i686_idt_entry {
    uint16_t offset_low;
    uint16_t segment;
    uint8_t  reserved;
    uint8_t  type : 4;
    uint8_t  null : 1;
    uint8_t  dpl  : 2;
    uint8_t  p    : 1;
    uint16_t offset_high;
} __attribute__((packed)) i686_idt_entry_t;

typedef struct i686_idt_pointer {
    uint16_t size;
    i686_idt_entry_t*   base;
} __attribute__((packed)) i686_idt_pointer_t;

_Static_assert(sizeof(i686_idt_entry_t) == 8, "i686_idt_entry_t musst be 8 bytes long");
_Static_assert(sizeof(i686_idt_pointer_t) == 6, "i686_idt_pointer_t musst be 6 bytes long");

void i686_idt_init(void) { ; }
void i686_idt_load(void) { ; }