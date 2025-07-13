#pragma once
#include <kernel/terminal.h>

extern const display_driver vga_driver;

bool i686_VGA_check();
void i686_VGA_activate();
void i686_VGA_deactivate();
void i686_VGA_putc(char c);
void i686_VGA_out(char c);
void i686_VGA_newLine();
void i686_VGA_parserSequnez(char operation);
void i686_VGA_sequenz_setGraphicsMode(uint16_t* params, uint16_t param_count);
void i686_VGA_scroll(uint8_t lines);
void i686_VGA_clear();

void i686_VGA_cursor_right();
void i686_VGA_cursor_left();
void i686_VGA_cursor_up();
void i686_VGA_cursor_down();
void i686_VGA_cursor_set(uint8_t x, uint8_t y);
