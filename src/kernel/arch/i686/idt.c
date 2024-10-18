#include <stdint.h>
#include <kernel/util/assembly.h>
#include <kernel/util/binnary.h>
#include "idt.h"

typedef struct {
    uint16_t BaseLow;
    uint16_t SegmentSelector;
    uint8_t  Reserved;
    uint8_t  Flags;
    uint16_t BaseHigh;
} ASMPACK IDTEntry;

typedef struct {
    uint16_t Limit;
    IDTEntry* Ptr;
} ASMPACK IDTDescriptor;

static IDTEntry IDT[256];
static IDTDescriptor IDT_DESCRIPTOR = {
    sizeof(IDT) -1, IDT
};

void ASMCALL i686_IDT_LOAD(IDTDescriptor* descriptor);

void i686_IDT_Initilize() {
    i686_IDT_LOAD(&IDT_DESCRIPTOR);
}

void i686_IDT_SetGate(uint8_t interrupt, void* base, uint16_t segmentDescriptor, uint8_t flags) {
    IDT[interrupt].BaseLow = ((uint32_t)base) & 0xFFFF;
    IDT[interrupt].SegmentSelector = segmentDescriptor;
    IDT[interrupt].Reserved = 0;
    IDT[interrupt].Flags = flags;
    IDT[interrupt].BaseHigh = ((uint32_t)base >> 16) & 0xFFFF;
}
void i686_IDT_EnableGate(uint8_t interrupt) {
    FLAG_SET(IDT[interrupt].Flags, IDT_FLAG_PRESENT);
}
void i686_IDT_DisableGate(uint8_t interrupt) {
    FLAG_UNSET(IDT[interrupt].Flags, IDT_FLAG_PRESENT);
}
