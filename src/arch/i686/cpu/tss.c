#include "tss.h"
#include "segments.h"

#include <stdint.h>

// Internal

extern uint32_t stack_top;

_Static_assert(sizeof(i686_tss_table_t) == 0x6C,
               "TSS table must be 0x6C bytes.");

i686_tss_table_t tss = {0};

// Definitions

void i686_tss_init(void) {
    tss = (i686_tss_table_t){
        .link  = 0x0,
        .esp0  = (uint32_t)&stack_top,
        .ss0   = SEGMENT(I686_KERNEL_DATA_SEGMENT, RPL0, TI_GDT),
        .cs    = SEGMENT(I686_KERNEL_CODE_SEGMENT, RPL0, TI_GDT),
        .es    = SEGMENT(I686_KERNEL_DATA_SEGMENT, RPL0, TI_GDT),
        .ss    = SEGMENT(I686_KERNEL_DATA_SEGMENT, RPL0, TI_GDT),
        .ds    = SEGMENT(I686_KERNEL_DATA_SEGMENT, RPL0, TI_GDT),
        .fs    = SEGMENT(I686_KERNEL_DATA_SEGMENT, RPL0, TI_GDT),
        .gs    = SEGMENT(I686_KERNEL_DATA_SEGMENT, RPL0, TI_GDT),
        .ldtr  = 0x0,
        .iopb  = sizeof(tss),
        .ssp   = 0x0,
    };
}
