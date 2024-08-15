#include "kernel.h"

static uint32_t* VideoMemory = (uint32_t*)0xb8000;
void __attribute__((cdecl)) kernel_main(void* multiboot_info_ptr) {
    VideoMemory[0] = 0x0f4b0f4f;
    while (1) ;
}