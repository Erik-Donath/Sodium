#include "terminal.h"
_Static_assert(sizeof(char) == sizeof(uint8_t), "char and uint8_t must be the same size!");

#include <kernel/arch/i686/vga.h>

void terminal_default_driver_init(void) { }
bool terminal_default_driver_check(void) { return true; }
void terminal_default_driver_putChar(char, uint8_t) { }
void terminal_default_driver_newLine() { }
void terminal_default_driver_scroll(uint8_t) {}
void terminal_default_driver_clear() {}

static const display_driver terminal_default_driver = {
    .name       = "Terminal Default Driver",
    .init       = terminal_default_driver_init,
    .check      = terminal_default_driver_check,
    .put_char   = terminal_default_driver_putChar,
    .new_line   = terminal_default_driver_newLine,
    .scroll     = terminal_default_driver_scroll,
    .clear      = terminal_default_driver_clear
};

static display_driver* terminal_driver = (display_driver*)&vga_driver;
static uint16_t terminal_color;

void terminal_init() {
    if(!terminal_driver->check()) {
        terminal_driver = (display_driver*)&terminal_default_driver;
    }
    
    terminal_driver->init();
}

void terminal_putchar(char c) {
    terminal_driver->put_char(c, terminal_color);
}

void terminal_write(const char* str) {
    while(*str) {
        terminal_putchar(*str);
        str++;
    }
}

void terminal_set_color(terminal_color_types fg, terminal_color_types bg) {
    terminal_color = Color(fg, bg);
}

void terminal_scroll(uint8_t lines) {
    terminal_driver->scroll(lines);
}

void terminal_clear() {
    terminal_driver->clear();
}

void terminal_status() {
    uint16_t color = terminal_color;
    terminal_set_color(TERMINAL_COLOR_LIGHT_GREY, TERMINAL_COLOR_BLACK);
    terminal_write("Terminal connected to '");
    terminal_write(terminal_driver->name);
    terminal_write("'\n");
    terminal_color = color;
}