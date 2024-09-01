#pragma once
#include <stdint.h>
#include <stdbool.h>

// Colors (Same as in arch/i686/vga.h defined)
#define BLACK          0x00
#define BLUE           0x01
#define GREEN          0x02
#define CYAN           0x03
#define RED            0x04
#define MAGENTA        0x05
#define BROWN          0x06
#define LIGHT_GRAY     0x07
#define DARK_GRAY      0x08
#define BRIGHT_BLUE    0x09
#define BRIGHT_GREEN   0x0A
#define BRIGHT_CYAN    0x0B
#define BRIGHT_RED     0x0C
#define BRIGHT_MAGENTA 0x0D
#define YELLOW         0x0E
#define WHITE          0x0F 

#define COLOR(f, b) (color)(((f) & 0x0F) | ((b) << 4))
#define DEFAULT_COLOR COLOR(LIGHT_GRAY, BLACK)

typedef uint16_t cchr; // colored char
typedef uint8_t color;

#define getColor(c) (color)((c) >> 8)
#define getChar(c) (char)((c) & 0x00FF)
#define CCHR(chr, clr) (cchr)((clr) << 8 | (chr))

void DISPLAY_Initilize();
void DISPLAY_clear();
bool DISPLAY_enabledebug();

void  DISPLAY_setcolor(color c);
color DISPLAY_getcolor();

void DISPLAY_debug(char c); // Send char to e9 port
void DISPLAY_nextc(char c); // Ignors escape chars [\n \r etc]
void DISPLAY_putc(char c);

void DISPLAY_setc(uint8_t x, uint8_t y, cchr c);
cchr DISPLAY_getc(uint8_t x, uint8_t y);

void DISPLAY_setcursor(uint8_t x, uint8_t y);
void DISPLAY_scroll(uint8_t lines);

uint8_t DISPLAY_getDisplayHeight();
uint8_t DISPLAY_getDisplayWidth();
