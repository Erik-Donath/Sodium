#include "../ports.h"
#include "debug.h"

const display_driver debug_driver = {
    .name       = "Debug Driver",
    .check      = i686_debug_check,
    .activate   = i686_debug_activate,
    .deactivate = i686_debug_deactivate,
    .putc       = i686_debug_putc,
    .clear      = i686_debug_clear
};

#define PORT 0xE9

bool i686_debug_check(void) {
    return true; // Cant check if E9 Hack is active. Just asuming it is.
}
void i686_debug_activate(void) { }
void i686_debug_deactivate(void) { }
void i686_debug_putc(char c) {
    outb(PORT, c);
}
void i686_debug_clear() {
    const char* str = "\033[2J";
    while(*str) {
        i686_debug_putc(*str);
        str++;
    }
}
