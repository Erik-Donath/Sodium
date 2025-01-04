#include "arch.h"

#include "fpu/fpu.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
#include "mb2/mb.h"

#include <kernel/terminal.h>

#define dc Color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK)
#define gc Color(TERMINAL_COLOR_GREEN, TERMINAL_COLOR_BLACK)
#define cc Color(TERMINAL_COLOR_CYAN, TERMINAL_COLOR_BLACK)

static void ok(const char* msg) {
    terminal_puts(dc "[ " gc "OK" dc " ] ");
    terminal_puts(msg);
    terminal_putc('\n');
}

static void okF(void(*func)(void), const char* name) {
    ok(name);
    func();
}

static void welcome() {
    terminal_puts("\033[0m" dc "Welcome to " cc "Sodium" dc "!\n");
}

void pre_main(void*) {
    terminal_init();
    welcome();

    ok("Terminal initialized");
    okF(i686_FPU_Initialize, "FPU initialized");
    okF(i686_GDT_Initialize, "GDT initialized");
    okF(i686_IDT_Initialize, "IDT initialized");

    // Color test
    terminal_putc('\n');
    terminal_testColor();
    terminal_puts("\nDONE!\n");

    while(true) {}
}
