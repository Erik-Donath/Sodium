#include <stdint.h>
#include <kernel/util/assembly.h>
#include "gdt.h"


typedef struct {
    uint16_t LimitLow;
    uint16_t BaseLow;
    uint8_t  BaseMiddle;
    uint8_t  Acess;
    uint8_t  FlagsLimitHi;
    uint8_t  BaseHigh;
} ASMPACK GDTEntry;

typedef struct {
    uint16_t Limit;
    GDTEntry* Ptr;
} ASMPACK GDTDescriptor;

typedef enum {
    GDT_ACCESS_CODE_READABLE            = 0x02,
    GDT_ACCESS_DATA_WRITEABLE           = 0x02,

    GDT_ACCESS_CODE_CONFORMING          = 0x04,
    GDT_ACCESS_DATA_DIRECTION_NORMAL    = 0x00,
    GDT_ACCESS_DATA_DIRECTION_DOWN      = 0x04,

    GDT_ACCESS_DATA_SEGMENT             = 0x10,
    GDT_ACCESS_CODE_SEGMENT             = 0x18,

    GDT_ACCESS_DESCRIPTOR_TSS           = 0x00,

    GDT_ACCESS_RING0                    = 0x00,
    GDT_ACCESS_RING1                    = 0x20,
    GDT_ACCESS_RING2                    = 0x40,
    GDT_ACCESS_RING3                    = 0x60,

    GDT_ACCESS_PRESENT                  = 0x80,
} GDT_ACCESS;

typedef enum {
    GDT_FLAG_64BIT                      = 0x20,
    GDT_FLAG_32BIT                      = 0x40,
    GDT_FLAG_16BIT                      = 0x00,

    GDT_FLAG_GRANULARITY_1B             = 0x00,
    GDT_FLAG_GRANULARITY_4K             = 0x80,
} GDT_FLAGS;

// The GDT Entry is a mess!
#define GDT_ENTRY(base, limit, access, flags) { \
    (limit & 0xFFFF),                           \
    (base  & 0xFFFF),                           \
    ((base >> 16) & 0xFF),                      \
    (access),                                   \
    (((limit >> 16) & 0xF) | (flags & 0xF0)),   \
    ((base >> 24) & 0xFF)                       \
}

static GDTEntry GDT[] = {
    // NULL-Descriptor
    GDT_ENTRY(0x00000, 0x00000,
        (0), 
        (0)
    ),
    // Kernel 32-bit code segment
    GDT_ENTRY(0x00000, 0xFFFFF, 
        (GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE),
        (GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K)
    ),
    // Kernel 32-bit data segment
    GDT_ENTRY(
        0x00000, 0xFFFFF,
        (GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITEABLE),
        (GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K)
    ),
    // Task State Segment (NOT IMPLEMENTED YET) => NULL-Desciptor
    GDT_ENTRY(0x00000, 0x00000,
        (0),
        (0)
    )
};

static GDTDescriptor GDT_DESCRIPTOR = {
    sizeof(GDT) - 1, GDT
};

void ASMCALL i686_GDT_LOAD(GDTDescriptor* desciptor, uint16_t codeSegment, uint16_t dataSegment);
void i686_GDT_Initialize() {
    i686_GDT_LOAD(&GDT_DESCRIPTOR, i686_GDT_CODE_SEGMENT, i686_GDT_DATA_SEGMENT);
}
