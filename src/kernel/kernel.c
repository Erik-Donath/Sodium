#include <stdint.h>

#include "hal/display.h"
#include "stdio.h"
#include "kernel.h"

void kernel_welcome() {
    DISPLAY_setcolor(COLOR(BRIGHT_CYAN, BLACK));
    printf("Welcome to Sodium!\n");
    DISPLAY_setcolor(DEFAULT_COLOR);
}

void printf_test() {
    printf("prinf Test:\n");
    printf("%%%% -> %%\n");
    printf("%%s -> %s\n", "String");
    printf("%%c -> %c\n", 'C');
    printf("%%d, %%i -> %d, %i\n", 42, -36);
    printf("%%u -> %u\n", 1337);
    printf("%%o -> %o\n", 34);
    printf("%%X -> %X\n", 4919);
    printf("%%B -> %B\n", 42);
    printf("%%p -> %p\n", 0x4242);
    printf("%%n -> %n\n");
}

void kernel_main(void* multiboot_info_ptr) {
    DISPLAY_init();
    kernel_welcome();

    while(1) ;
}
