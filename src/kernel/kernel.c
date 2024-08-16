#include "kernel.h"
#include "arch/i686/vga.h"

void __attribute__((cdecl)) kernel_main(void* multiboot_info_ptr) {
    VGA_setcolor(VGA_BACK(VGA_COLOR_BLACK) | VGA_FRONT(VGA_COLOR_BRIGHT_CYAN));
    VGA_clrscr();
    VGA_puts("Welcome to Sodium!\n");
    VGA_setcolor(VGA_DEFAULT_COLOR);

    while(1);
}