#include <stdint.h>
#include <kernel/hal/hal.h>

#include "hal/display.h"
#include "stdio.h"
#include "kernel.h"

void kernel_main(void* mb_info) {
    HAL_Initilize();

    // Display welcome msg and tell user if e9 hack is enabled.
    DISPLAY_setcolor(COLOR(BRIGHT_CYAN, BLACK));
    puts("Welcome to Sodium!\n");
    DISPLAY_setcolor(DEFAULT_COLOR);

    while(1) ;
}
