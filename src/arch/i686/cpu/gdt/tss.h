#pragma once

#include <stdint.h>
#include <kernel/util.h>

typedef struct {
    uint16_t prev_tss; // Link to the previous task
    uint16_t reserved0;
    uint32_t esp0;               // Stack pointer for ring 0
    uint16_t ss0;                // Stack segment for ring 0
    uint16_t reserved1;
    uint32_t esp1;               // Stack pointer for ring 1
    uint16_t ss1;                // Stack segment for ring 1
    uint16_t reserved2;
    uint32_t esp2;               // Stack pointer for ring 2
    uint16_t ss2;                // Stack segment for ring 2
    uint16_t reserved3;
    uint32_t cr3;                // Page directory base register
    uint32_t eip;                // Instruction pointer
    uint32_t eflags;             // Flags register
    uint32_t eax;                // General-purpose registers
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;                // Stack pointer
    uint32_t ebp;                // Base pointer
    uint32_t esi;                // Source index
    uint32_t edi;                // Destination index
    uint16_t es;                 // Segment selectors
    uint16_t reserved4;
    uint16_t cs;
    uint16_t reserved5;
    uint16_t ss;
    uint16_t reserved6;
    uint16_t ds;
    uint16_t reserved7;
    uint16_t fs;
    uint16_t reserved8;
    uint16_t gs;
    uint16_t reserved9;
    uint16_t ldt_segment_selector; // Local Descriptor Table segment selector
    uint16_t reserved10;
    uint16_t debug_trap : 1;     // Debug trap flag
    uint16_t reserved11 : 15;
    uint16_t io_map_base_address; // I/O map base address
} PACKED tss_entry;

extern tss_entry i686_tss;