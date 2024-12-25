#include "arch.h"

#include <kernel/terminal.h>
#include "fpu.h"
#include "gdt.h"

static void ok(const char* msg) {
    terminal_set_color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK);
    terminal_write("[ ");
    terminal_set_color(TERMINAL_COLOR_GREEN, TERMINAL_COLOR_BLACK);
    terminal_write("OK");
    terminal_set_color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK);
    terminal_write(" ] ");
    terminal_write(msg);
    terminal_putchar('\n');
}

static void okF(void(*func)(void), const char* name) {
    ok(name);
    func();
}

static void welcome() {
    terminal_set_color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK);
    terminal_write("Welcome to ");
    terminal_set_color(TERMINAL_COLOR_CYAN, TERMINAL_COLOR_BLACK);
    terminal_write("Sodium");
    terminal_set_color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK);
    terminal_write("!\n");
}

void pre_main(void*) {
    terminal_init();
    welcome();

    ok("Terminal initialized");
    terminal_status();
    
    okF(i686_FPU_Initialize, "FPU initialized");
    okF(i686_GDT_Initialize, "GDT initialized");

    while(true) {}
}
