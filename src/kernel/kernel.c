#include <stdint.h>

#include "hal/display.h"
#include "stdio.h"
#include "kernel.h"

void kernel_welcome() {
    DISPLAY_setcolor(COLOR(BRIGHT_CYAN, BLACK));
    puts("Welcome to Sodium!\n");
    DISPLAY_setcolor(DEFAULT_COLOR);
}

void kernel_main(void* mb_info) {
    DISPLAY_init();
    kernel_welcome();

    SYSINFO_parse_multiboot(mb_info);
    
    while(1) ;
}
