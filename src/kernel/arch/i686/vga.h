#pragma once
#include <stdint.h>

#define VGA_SCREEN_BUFFER_ADDRESS 0xb8000
#define VGA_SCREEN_WIDTH  80
#define VGA_SCREEN_HEIGHT 25
#define VGA_DEFAULT_COLOR 0x7

#define VGA_COLOR_BLACK          0x00
#define VGA_COLOR_BLUE           0x01
#define VGA_COLOR_GREEN          0x02
#define VGA_COLOR_CYAN           0x03
#define VGA_COLOR_RED            0x04
#define VGA_COLOR_MAGENTA        0x05
#define VGA_COLOR_BROWN          0x06
#define VGA_COLOR_LIGHT_GRAY     0x07
#define VGA_COLOR_DARK_GRAY      0x08
#define VGA_COLOR_BRIGHT_BLUE    0x09
#define VGA_COLOR_BRIGHT_GREEN   0x0A
#define VGA_COLOR_BRIGHT_CYAN    0x0B
#define VGA_COLOR_BRIGHT_RED     0x0C
#define VGA_COLOR_BRIGHT_MAGENTA 0x0D
#define VGA_COLOR_YELLOW         0x0E
#define VGA_COLOR_WHITE          0x0F

#define VGA_FRONT(x) (0x0F & (x << 0))
#define VGA_BACK(x)  (0xF0 & (x << 4))

void VGA_setchr(uint8_t x, uint8_t y, char c, uint8_t color);
char VGA_getchr(uint8_t x, uint8_t y);
uint8_t VGA_getcolor(uint8_t x, uint8_t y);

void VGA_clrscr();
void VGA_scrollback(uint8_t lines);

void VGA_setcursor(uint8_t x, uint8_t y);
void VGA_advancecursor();

void VGA_putchr(char c, uint8_t color);
void VGA_putc(char c);
void VGA_puts(const char* str);

void VGA_setcolor(uint8_t color);
uint8_t VGA_getcurcolor();
uint8_t VGA_getcursorx();
uint8_t VGA_getcursory();

// See also: https://wiki.osdev.org/VGA_Hardware