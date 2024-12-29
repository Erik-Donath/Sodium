#include "ports.h"
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
/*
void debug_putc(char c) {
    outb(PORT, c);
}
void debug_write(const char* str) {
    while(*str) {
        debug_putc(*str);
        str++;
    }
}

const char* ansi_color_codes[] = {
    "0", "4", "2", "6", "1", "5", "3", "7",       // 0-7 standard colors (black, blue, green, cyan, red, magenta, brown, white)
    "8", "12", "10", "14", "9", "13", "11", "15"  // 8-15 bright colors (gray, light blue, light green, light cyan, light red, light magenta, yellow, bright white)
};

void debug_color(uint8_t color) {
    uint8_t fg = color & 0x0F;
    uint8_t bg = (color >> 4) & 0x0F;

    debug_write("\033[38;5;");
    debug_write(ansi_color_codes[fg]);
    debug_write("m\033[48;5;");
    debug_write(ansi_color_codes[bg]);
    debug_putc('m');
}
*/