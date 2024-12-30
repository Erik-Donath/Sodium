#pragma once
#include <kernel/terminal.h>

extern const display_driver vga_driver;

bool i686_vga_check();
void i686_vga_activate();
void i686_vga_deactivate();
void i686_vga_putc(char c);
void i686_vga_out(char c);
void i686_vga_cursor_forward();
void i686_vga_cursor_backward();
void i686_vga_set_cursor(uint8_t x, uint8_t y);
void i686_vga_scroll(uint8_t lines);
void i686_vga_clear();
