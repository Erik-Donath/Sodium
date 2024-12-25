#include "arch.h"

#include <kernel/terminal.h>
#include "fpu.h"
#include "gdt.h"

static void ok(void(*func)(void), const char* name) {
    func();
    terminal_set_color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK);
    terminal_write("[ ");
    terminal_set_color(TERMINAL_COLOR_GREEN, TERMINAL_COLOR_BLACK);
    terminal_write("OK");
    terminal_set_color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK);
    terminal_write(" ] ");
    terminal_write(name);
    terminal_putchar('\n');
}

static void welcome() {
    terminal_set_color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK);
    terminal_write("Welcome to ");
    terminal_set_color(TERMINAL_COLOR_CYAN, TERMINAL_COLOR_BLACK);
    terminal_write("Sodium");
    terminal_set_color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK);
    terminal_write("!\n\n");
}

void pre_main(void*) {
    terminal_init();
    welcome();

    ok(i686_FPU_Initialize, "FPU initialized");
    ok(i686_GDT_Initialize, "GDT initialized");

    while(true) {}
}
