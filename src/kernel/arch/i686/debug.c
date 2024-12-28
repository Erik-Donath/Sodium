#include "ports.h"
#include "debug.h"

#define PORT 0xE9

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