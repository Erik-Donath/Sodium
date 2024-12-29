#include "arch.h"

#include <kernel/terminal.h>
#include "fpu/fpu.h"
#include "gdt/gdt.h"
#include "idt/idt.h"

#define dc Color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK)
#define gc Color(TERMINAL_COLOR_GREEN, TERMINAL_COLOR_BLACK)
#define cc Color(TERMINAL_COLOR_CYAN, TERMINAL_COLOR_BLACK)

static void ok(const char* msg) {
    terminal_puts(dc "[ " gc "OK" dc);
    terminal_puts(msg);
    terminal_putc('\n');
}

static void okF(void(*func)(void), const char* name) {
    ok(name);
    func();
}

static void welcome() {
    terminal_puts(dc "Welcome to " cc "Sodium" dc "!\n");
}

void pre_main(void*) {
    terminal_init();
    welcome();

    ok("Terminal initialized");
    terminal_status();

    okF(i686_FPU_Initialize, "FPU initialized");
    okF(i686_GDT_Initialize, "GDT initialized");
    okF(i686_IDT_Initialize, "IDT initialized");

    /*
    __asm__ volatile ("int $0");
    __asm__ volatile ("int $1");
    __asm__ volatile ("int $2");
    __asm__ volatile ("int $3");
    __asm__ volatile ("int $4");
    __asm__ volatile ("int $5");
    __asm__ volatile ("int $6");
    __asm__ volatile ("int $7");
    __asm__ volatile ("int $9");
    __asm__ volatile ("int $15");
    __asm__ volatile ("int $16");
    __asm__ volatile ("int $18");
    __asm__ volatile ("int $19");
    __asm__ volatile ("int $20");
    __asm__ volatile ("int $21");
    __asm__ volatile ("int $22");
    __asm__ volatile ("int $23");
    __asm__ volatile ("int $24");
    __asm__ volatile ("int $25");
    __asm__ volatile ("int $26");
    __asm__ volatile ("int $27");
    __asm__ volatile ("int $28");
    __asm__ volatile ("int $29");
    __asm__ volatile ("int $31");
    */

    __asm__ volatile ("int $0");
    __asm__ volatile ("int $255");

    terminal_puts("\nDONE!\n");
    while(true) {}
}
