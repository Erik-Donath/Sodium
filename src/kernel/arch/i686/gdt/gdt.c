#include <stddef.h>
#include <stdint.h>

#include <kernel/util.h>
#include "gdt.h"

/*
| Bits    | Description                               |
|---------|-------------------------------------------|
| 0-15    | Segment Limit (lower 16 bits)             |
| 16-31   | Base Address (lower 16 bits)              |
| 32-39   | Base Address (middle 8 bits)              |
| 40-47   | Access Rights                             |
| 48-51   | Segment Limit (upper 4 bits)              |
| 52-55   | Flags                                     |
| 56-63   | Base Address (upper 8 bits)               |
*/

typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} PACKED gdt_entry;

typedef struct {
    uint16_t limit;
    gdt_entry* base;
} PACKED gdt_pointer;

typedef enum {
    // P (Present) bit
    GDT_ACCESS_PRESENT                  = 0x80,

    // DPL (Descriptor Privilege Level) bits
    GDT_ACCESS_RING0                    = 0x00,
    GDT_ACCESS_RING1                    = 0x20,
    GDT_ACCESS_RING2                    = 0x40,
    GDT_ACCESS_RING3                    = 0x60,

    // S (Descriptor type) bit
    GDT_ACCESS_SYSTEM                   = 0x00,
    GDT_ACCESS_CODE_DATA                = 0x10,

    // E (Executable) bit
    GDT_ACCESS_EXECUTABLE               = 0x08,

    // DC (Direction/Conforming) bit
    GDT_ACCESS_DATA_DIRECTION_DOWN      = 0x04,
    GDT_ACCESS_CODE_CONFORMING          = 0x04,

    // RW (Readable/Writable) bit
    GDT_ACCESS_CODE_READABLE            = 0x02,
    GDT_ACCESS_DATA_WRITEABLE           = 0x02,

    // A (Accessed) bit
    GDT_ACCESS_ACCESSED                 = 0x01,

    // Commonly used combinations
    GDT_ACCESS_DATA_SEGMENT             = GDT_ACCESS_CODE_DATA,
    GDT_ACCESS_CODE_SEGMENT             = GDT_ACCESS_CODE_DATA | GDT_ACCESS_EXECUTABLE,
} GDT_ACCESS;

typedef enum {
    // G (Granularity) flag
    GDT_FLAG_GRANULARITY_1B             = 0x00,
    GDT_FLAG_GRANULARITY_4K             = 0x80,

    // D/B (Default operation size / Default stack pointer size and/or upper bound) flag
    GDT_FLAG_16BIT                      = 0x00,
    GDT_FLAG_32BIT                      = 0x40,

    // L (64-bit code segment) flag
    GDT_FLAG_64BIT                      = 0x20,

    // AVL (Available for system use) flag
    GDT_FLAG_AVAILABLE                  = 0x10
} GDT_FLAGS;

#define GDT_ENTRY(base, limit, access, flags) { \
    (uint16_t)((limit) & 0xFFFF), \
    (uint16_t)((base) & 0xFFFF), \
    (uint8_t)(((base) >> 16) & 0xFF), \
    (uint8_t)(access), \
    (uint8_t)(((limit) >> 16) & 0x0F) | (uint8_t)((flags) & 0xF0), \
    (uint8_t)(((base) >> 24) & 0xFF) \
}


// GDT Table
static gdt_entry gdt[] = {
    // Null descriptor
    GDT_ENTRY(0, 0, 0, 0),
    
    // Kernel 32-bit code segment
    GDT_ENTRY(0, 0xFFFFF, 
        GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE,
        GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K
    ),
    
    // Kernel 32-bit data segment
    GDT_ENTRY(0, 0xFFFFF,
        GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE,
        GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K
    )
};

// GDT pointer
static gdt_pointer gdt_ptr = {
    .limit = sizeof(gdt) - 1,
    .base = gdt
};

// Assert that Segment positions are correct
#define GDT_ASSERT(segment, index, expected) _Static_assert(offsetof(gdt_entry, limit_low) + sizeof(gdt_entry) * index == expected, segment " offset mismatch.")
GDT_ASSERT("Kernel code segment", 1, GDT_KERNEL_CODE_SEGMENT);
GDT_ASSERT("Kernel data segment", 2, GDT_KERNEL_DATA_SEGMENT);

extern void ASM_CALL i686_GDT_Flush(gdt_pointer* ptr, uint16_t codeSegment, uint16_t dataSegment);
void i686_GDT_Initialize() { i686_GDT_Flush(&gdt_ptr, GDT_KERNEL_CODE_SEGMENT, GDT_KERNEL_DATA_SEGMENT); }
