#pragma once
#include <kernel/terminal.h>

extern const display_driver vga_driver;

void i686_vga_init(void);
bool i686_vga_check(void);
void i686_vga_putChar(char c, uint8_t color);
void i686_vga_newLine(void);
void i686_vga_scroll(uint8_t lines);
void i686_vga_clear();

void i686_vga_out(char c, uint8_t color);
void i686_vga_set_cursor_position(uint16_t pos);