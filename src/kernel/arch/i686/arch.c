#include "arch.h"

#include "fpu/fpu.h"
#include "gdt/gdt.h"
#include "idt/idt.h"

#include <kernel/terminal.h>

#define dc Color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK)
#define ec Color(TERMINAL_COLOR_RED, TERMINAL_COLOR_BLACK)
#define gc Color(TERMINAL_COLOR_GREEN, TERMINAL_COLOR_BLACK)
#define cc Color(TERMINAL_COLOR_CYAN, TERMINAL_COLOR_BLACK)

static void ok(const char* msg) {
    terminal_puts(dc "[ " gc "OK" dc " ] ");
    terminal_puts(msg);
    terminal_putc('\n');
}

static void failed(const char* msg) {
    terminal_puts(dc "[ " ec "FAILED" dc " ] ");
    terminal_puts(msg);
    terminal_putc('\n');
}

static void welcome() {
    terminal_puts("\033[0m" dc "Welcome to " cc "Sodium" dc "!\n");
}

void pre_main(mb_info_ptr mb) {
    // Setup CPU
    i686_FPU_Initialize();

    // Setup Terminal
    terminal_init();
    welcome();
    ok("Terminal initialized");

    // Setup CPU Tables
    i686_GDT_Initialize();
    i686_IDT_Initialize();
    ok("CPU Tables initialized");
    // Parse multiboot Information
    bool success = mb_parse(mb);
    if(!success) {
        failed("Failed to load multiboot info");
        return;
    }
    ok("Loaded multiboot info");
    terminal_putc('\n');
    mb_print(mb);

    // Color test
    terminal_putc('\n');
    terminal_testColor();
    terminal_puts("\nDONE!\n");


    while(true) {}
}
