#include <stdint.h>
#include <kernel/hal/hal.h>

#include "hal/display.h"
#include "stdio.h"
#include "kernel.h"

#include "arch/i686/isr.h"
#include "arch/i686/irq.h"

void timer(ISR_Registers* regs) {
    putc('.');
}

void kernel_main() {
    HAL_Initilize();
    kernel_welcome();

    i686_IRQ_RegisterHandler(0, timer);
    while(1) ;
}

void kernel_welcome() {
    /*
    if(DISPLAY_enabledebug()) {
        DISPLAY_setcolor(COLOR(DARK_GRAY, BLACK));
        DISPLAY_setcursor(54, 0);
        puts("[CONSOLE DEBUGING ENABLED]\n");
    }*/

    DISPLAY_setcursor(0, 0);

    DISPLAY_setcolor(COLOR(WHITE, BLACK));
    puts("Welcome to ");

    DISPLAY_setcolor(COLOR(BRIGHT_CYAN, BLACK));
    puts("Sodium");

    DISPLAY_setcolor(COLOR(WHITE, BLACK));
    puts("!\n");

    DISPLAY_setcolor(DEFAULT_COLOR);
}
