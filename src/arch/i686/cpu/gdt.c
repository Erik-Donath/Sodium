#include "gdt.h"
#include "tss.h"
#include "segments.h"

#include <stdint.h>

// Internal
//
// GDT entry layout:
//  Bits  0-15  Segment limit  (low 16)
//  Bits 16-31  Base address   (low 16)
//  Bits 32-39  Base address   (mid  8)
//  Bits 40-47  Access byte
//  Bits 48-51  Segment limit  (high 4)
//  Bits 52-55  Flags
//  Bits 56-63  Base address   (high 8)
// See: https://wiki.osdev.org/Global_Descriptor_Table

enum {
    // Access byte bits
    GDT_ACCESS_PRESENT              = 0x80,
    GDT_ACCESS_RING0                = 0x00,
    GDT_ACCESS_RING1                = 0x20,
    GDT_ACCESS_RING2                = 0x40,
    GDT_ACCESS_RING3                = 0x60,
    GDT_ACCESS_SYSTEM               = 0x00,
    GDT_ACCESS_CODE_DATA            = 0x10,
    GDT_ACCESS_EXECUTABLE           = 0x08,
    GDT_ACCESS_DATA_DIRECTION_DOWN  = 0x04,
    GDT_ACCESS_CODE_CONFORMING      = 0x04,
    GDT_ACCESS_CODE_READABLE        = 0x02,
    GDT_ACCESS_DATA_WRITEABLE       = 0x02,
    GDT_ACCESS_ACCESSED             = 0x01,

    // Flag nibble bits
    GDT_FLAG_GRANULARITY_1B         = 0x00,
    GDT_FLAG_GRANULARITY_4K         = 0x08,
    GDT_FLAG_16BIT                  = 0x00,
    GDT_FLAG_32BIT                  = 0x04,
    GDT_FLAG_64BIT                  = 0x02,
};

typedef struct i686_gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  limit_high : 4;
    uint8_t  flags      : 4;
    uint8_t  base_high;
} __attribute__((packed)) i686_gdt_entry_t;

typedef struct i686_gdt_pointer {
    uint16_t         limit;
    i686_gdt_entry_t *base;
} __attribute__((packed)) i686_gdt_pointer_t;

_Static_assert(sizeof(i686_gdt_entry_t)   == 8, "i686_gdt_entry_t must be 8 bytes.");
_Static_assert(sizeof(i686_gdt_pointer_t) == 6, "i686_gdt_pointer_t must be 6 bytes.");

#define ENTRY_COUNT 6

static i686_gdt_entry_t gdt[ENTRY_COUNT] = {0};
static i686_gdt_pointer_t gdt_ptr = {
    .limit = sizeof(gdt) - 1,
    .base  = gdt,
};

extern void __attribute__((cdecl)) i686_gdt_flush(i686_gdt_pointer_t *ptr);

// @brief Write one GDT descriptor slot.
// @param segnum  Slot index (0–5).
// @param base    Segment base address.
// @param limit   Segment limit (20-bit; granularity controls unit).
// @param access  Access byte (present, DPL, type bits).
// @param flags   Flag nibble (granularity, size, long-mode bits).
static void i686_gdt_set(uint8_t segnum, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);

// Definitions

static void i686_gdt_set(uint8_t segnum, uint32_t base, uint32_t limit,
                          uint8_t access, uint8_t flags) {
    gdt[segnum] = (i686_gdt_entry_t){
        .limit_low   = (uint16_t)(limit & 0xFFFF),
        .base_low    = (uint16_t)(base  & 0xFFFF),
        .base_middle = (uint8_t)((base  >> 16) & 0xFF),
        .access      = access,
        .limit_high  = (uint8_t)((limit >> 16) & 0x0F),
        .flags       = (uint8_t)(flags & 0x0F),
        .base_high   = (uint8_t)((base  >> 24) & 0xFF),
    };
}

void i686_gdt_init(void) {
     // Null descriptor
    i686_gdt_set(0, 0x00000, 0x00000,
        0,
        0
    );

    // Kernel code
    i686_gdt_set(1, 0x00000, 0xFFFFF,
        GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_DATA | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_CODE_READABLE,
        GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K
    );

    // Kernel data
    i686_gdt_set(2, 0x00000, 0xFFFFF,
        GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_DATA | GDT_ACCESS_DATA_WRITEABLE,
        GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K
    );

    // User code
    i686_gdt_set(3, 0x00000, 0xFFFFF,
        GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_CODE_DATA | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_CODE_READABLE,
        GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K
    );

    // User data
    i686_gdt_set(4, 0x00000, 0xFFFFF,
        GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_CODE_DATA | GDT_ACCESS_DATA_WRITEABLE,
        GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K
    );

    // TSS
    i686_gdt_set(5, (uint32_t)&tss, (uint32_t)(sizeof(i686_tss_table_t) - 1),
        GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_SYSTEM | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_ACCESSED,
        GDT_FLAG_32BIT
    );
}

void i686_gdt_load(void) {
    i686_gdt_flush(&gdt_ptr);
}
