#include "terminal.h"
_Static_assert(sizeof(char) == sizeof(uint8_t), "char and uint8_t must be the same size!");

#include <kernel/arch/i686/vga/vga.h>
#include <kernel/arch/i686/debug.h>

bool terminal_default_driver_check(void) { return true; }
void terminal_default_driver_activate(void) { }
void terminal_default_driver_deactivate(void) { }
void terminal_default_driver_putc(char) { }
void terminal_default_driver_clear() {}

static const display_driver terminal_default_driver = {
    .name       = "Terminal Default Driver",
    .check      = terminal_default_driver_check,
    .activate   = terminal_default_driver_activate,
    .deactivate = terminal_default_driver_deactivate,
    .putc       = terminal_default_driver_putc,
    .clear      = terminal_default_driver_clear
};

static display_driver* terminal_driver = (display_driver*)&vga_driver;

void terminal_init() {
    if(!terminal_driver->check()) {
        terminal_driver = (display_driver*)&terminal_default_driver;
    }
    terminal_driver->activate();
}

void terminal_putc(char c) {
    terminal_driver->putc(c);
}

void terminal_puts(const char* str) {
    while(*str) {
        terminal_putc(*str);
        str++;
    }
}

void terminal_clear() {
    terminal_driver->clear();
}

void terminal_status() {
    terminal_puts(Color(TERMINAL_COLOR_LIGHT_GREY, TERMINAL_COLOR_BLACK) "Terminal connected to '");
    terminal_puts(terminal_driver->name);
    terminal_puts("'" Color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK) "\n");
}