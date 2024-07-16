#include "gdt.h"
#include "kernel.h"

uint32_t* VideoMemory = (uint32_t*)0xb8000;

void kernel_main(uint32_t* mb_info_struct) {
    GDT_Initialize();

    VideoMemory[0] = 0x0f4b0f4f;
    while (1) ;
}
