#include <stdint.h>

#include "hal/display.h"
#include "stdio.h"
#include "kernel.h"

void kernel_welcome() {
    DISPLAY_setcolor(COLOR(BRIGHT_CYAN, BLACK));
    printf("Welcome to Sodium!\n");
    DISPLAY_setcolor(DEFAULT_COLOR);
}

void kernel_main(void* multiboot_info_ptr) {
    DISPLAY_init();
    kernel_welcome();
    printf_test();

    while(1) ;
}
