#include "arch/i686/vga.h"
#include "kernel.h"

void kernel_main(void* multiboot_info_ptr) {
    //VGA_init();
    VGA_clrscr();

    // Print Welcome Message
    VGA_setcolor(VGA_BACK(VGA_COLOR_BLACK) | VGA_FRONT(VGA_COLOR_BRIGHT_CYAN));
    VGA_puts("Welcome to Sodium!\n");
    VGA_setcolor(VGA_DEFAULT_COLOR);

    while(1);
}