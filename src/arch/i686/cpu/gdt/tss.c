#include <stdint.h>
#include <kernel/util.h>

#include "tss.h"
#include "gdt.h"

enum {
    RPL0 = 0,
    RPL1 = 1,
    RPL2 = 2,
    RPL3 = 3
};

enum {
    TI_GDT  = 0x00,
    TI_LDT  = 0x04,
};

tss_entry i686_tss;

void i686_TSS_Flush(uint16_t tssSegment);

void i686_TSS_Init() {
    memset(&i686_tss, 0, sizeof(tss_entry));
    i686_tss.ss0 = GDT_KERNEL_DATA_SEGMENT;
    i686_tss.esp0 = 0;
    
    i686_tss.cs  = GDT_KERNEL_CODE_SEGMENT | RPL3;
    i686_tss.ds = i686_tss.es = i686_tss.fs = i686_tss.gs = i686_tss.ss = GDT_KERNEL_DATA_SEGMENT | RPL3;

    i686_TSS_Flush(GDT_TSS_SEGMENT | TI_GDT | RPL3);    
}
