#include <kernel/arch/i686/panic.h>

#include "display.h"
#include "panic.h"

void kernel_panic() {
    const char* msg = "\nKERNEL PANIC!";
    DISPLAY_setcolor(COLOR(RED, BLACK));
    while(*msg) {
        DISPLAY_putc(*msg);
        msg++;
    }

    i686_panic();
}
