#pragma once

#include <stdint.h>

// Task State Segment table layout.
// See: https://wiki.osdev.org/Task_State_Segment

typedef struct i686_tss_table {
    uint16_t link;       uint16_t reserved0;
    uint32_t esp0;
    uint16_t ss0;        uint16_t reserved1;
    uint32_t esp1;
    uint16_t ss1;        uint16_t reserved2;
    uint32_t esp2;
    uint16_t ss2;        uint16_t reserved3;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint16_t es;         uint16_t reserved4;
    uint16_t cs;         uint16_t reserved5;
    uint16_t ss;         uint16_t reserved6;
    uint16_t ds;         uint16_t reserved7;
    uint16_t fs;         uint16_t reserved8;
    uint16_t gs;         uint16_t reserved9;
    uint16_t ldtr;       uint16_t reserved10;
    uint16_t reserved11; uint16_t iopb;
    uint32_t ssp;
} __attribute__((packed)) i686_tss_table_t;

// The single TSS instance; defined in tss.c.
// TODO: This will be a large array in the Future when mutli processor mode is established
extern i686_tss_table_t tss;

// @brief Zero-init the TSS and set kernel stack pointer (esp0 / ss0).
void i686_tss_init(void);

// @brief Load TSS selector into the Task Register (LTR 0x28).
void __attribute__((cdecl)) i686_tss_load(void);
