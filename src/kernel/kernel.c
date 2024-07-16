#include "kernel.h"

void kernel_main(uint32_t* mb_info_struct) {
    uint32_t* VideoMemory = (uint32_t*)0xb8000;
    VideoMemory[0] = 0x0f4b0f4f;
    while (1) ;
}
