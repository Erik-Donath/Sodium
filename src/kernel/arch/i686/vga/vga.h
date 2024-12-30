#pragma once
#include <kernel/terminal.h>

extern const display_driver vga_driver;

bool i686_vga_check();
void i686_vga_activate();
void i686_vga_deactivate();
void i686_vga_putc(char c);
void i686_vga_out(char c);
void i686_vga_parserSequnez(char operation);
void i686_vga_scroll(uint8_t lines);
void i686_vga_clear();

void i686_vga_cursor_right();
void i686_vga_cursor_left();
void i686_vga_cursor_up();
void i686_vga_cursor_down();
void i686_vga_cursor_set(uint8_t x, uint8_t y);
