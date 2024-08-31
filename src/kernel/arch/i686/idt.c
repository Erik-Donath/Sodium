#include <stdint.h>
#include <kernel/util/assembly.h>
#include <kernel/util/binnary.h>
#include "idt.h"

typedef struct {
    uint16_t BaseLow;
    uint16_t SegmentSelector;
    uint8_t  Reserved;
    uint8_t  Flags;
    uint8_t  BaseHight;
} ASMPACK IDTEntry;

typedef struct {
    uint16_t Limit;
    IDTEntry* Ptr;
} ASMPACK IDTDescriptor;

static IDTEntry IDT[256];
static IDTDescriptor IDT_DESCRIPTOR = {
    sizeof(IDT) -1, IDT
};

#define IDT_ENTRY(base, segmentDescriptor, flags) { \
    ((uint32_t)base & 0xFFFF),                      \
    (segmentDescriptor),                            \
    0, (flags),                                     \
    ((uint32_t)base >> 16)                          \
}

void ASMCALL IDT_LOAD(IDTDescriptor* descriptor);

void IDT_Initilize() {
    IDT_LOAD(&IDT_DESCRIPTOR);
}

void IDT_SetGate(uint8_t interupt, void* base, uint16_t segmentDescriptor, uint8_t flags) {
    IDT[interupt] = (IDTEntry)IDT_ENTRY(base, segmentDescriptor, flags);
}
void IDT_EnableGate(uint8_t interupt) {
    FLAG_SET(IDT[interupt].Flags, IDT_FLAG_PRESENT);
}
void IDT_DisableGate(uint8_t interupt) {
    FLAG_UNSET(IDT[interupt].Flags, IDT_FLAG_PRESENT);
}