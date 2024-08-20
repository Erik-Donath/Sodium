#include <stdint.h>

#include "arch/i686/e9.h"
#include "hal/display.h"
#include "stdio.h"
#include "kernel.h"

void kernel_main(void* mb_info) {
    SYSINFO_parse_multiboot(mb_info);
    DISPLAY_init();

    // Display welcome msg and tell user if e9 hack is enabled.
    DISPLAY_setcolor(COLOR(BRIGHT_CYAN, BLACK));
    puts("Welcome to Sodium!\n");
    DISPLAY_setcolor(DEFAULT_COLOR);

    if(E9_test()) {
        DISPLAY_setcolor(COLOR(MAGENTA, BLACK));
        puts("E9 Hack Enabled\n");
        DISPLAY_setcolor(DEFAULT_COLOR);
    }
    putc('\n');

    while(1) ;
}
