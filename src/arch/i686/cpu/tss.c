#include "tss.h"
#include "gdt.h"
#include <stdint.h>

// Defined in boot.asm:
extern uint32_t stack_top;

// See: https://wiki.osdev.org/Task_State_Segment

typedef struct __attribute__((packed)) i686_tss_table {
  uint16_t link;
  uint16_t reserved0;

  uint32_t esp0;

  uint16_t ss0;
  uint16_t reserved1;

  uint32_t esp1;

  uint16_t ss1;
  uint16_t reserved2;

  uint32_t esp2;

  uint16_t ss2;
  uint16_t reserved3;

  uint32_t cr3;
  uint32_t eip;
  uint32_t eflags;
  uint32_t eax;
  uint32_t ecx;
  uint32_t edx;
  uint32_t ebx;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esi;
  uint32_t edi;

  uint16_t es;
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

  uint16_t ldtr;
  uint16_t reserved10;

  uint16_t reserved11;
  uint16_t iopb;

  uint32_t ssp;
} i686_tss_table;

// See: https://wiki.osdev.org/Segment_Selector
typedef enum i686_tss_rpl : uint16_t {
  RPL0 = 0x00,
  RPL1 = 0x01,
  RPL2 = 0x02,
  RPL3 = 0x03,
} i686_tss_rpl;

// See: https://wiki.osdev.org/Segment_Selector
typedef enum i686_tss_ti : uint16_t {
  TI_GDT = 0x00,
  TI_LDT = 0x04,
} i686_tss_ti;

_Static_assert(sizeof(i686_tss_table) == 0x6C,
               "TSS Table musst be the Size of 0x68 bytes!");

i686_tss_table tss = {0};

void i686_tss_init(void) {
  tss = (i686_tss_table){
      .link = 0x0,

      .esp0 = (uint32_t)&stack_top,
      .ss0 = i686_GDT_KERNEL_DATA_SEGMENT | RPL0 | TI_GDT,

      .cs = i686_GDT_KERNEL_CODE_SEGMENT | RPL0 | TI_GDT,
      .es = i686_GDT_KERNEL_DATA_SEGMENT | RPL0 | TI_GDT,
      .ss = i686_GDT_KERNEL_DATA_SEGMENT | RPL0 | TI_GDT,
      .ds = i686_GDT_KERNEL_DATA_SEGMENT | RPL0 | TI_GDT,
      .fs = i686_GDT_KERNEL_DATA_SEGMENT | RPL0 | TI_GDT,
      .gs = i686_GDT_KERNEL_DATA_SEGMENT | RPL0 | TI_GDT,

      .ldtr = 0x0,
      .iopb = sizeof(tss),
      .ssp = 0x0,
  };
}

void __attribute__((cdecl)) i686_tss_flush(
    uint16_t segment); // segment number from gdt; Defined in Assembly

void i686_tss_load(void) {
  // Flush TSS
  i686_tss_flush(i686_GDT_TSS_SEGMENT);
}