/* WARNING: This driver does not implement any Graphis yet. It is still in development */
// See also: https://wiki.osdev.org/VGA_Hardware

#pragma once
#include <stdint.h>

#define VGA_SCREEN_BUFFER_ADDRESS 0xb8000
#define VGA_SCREEN_WIDTH  80
#define VGA_SCREEN_HEIGHT 25

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

void VGA_Initilize();

uint8_t VGA_getScreenHeight();
uint8_t VGA_getScreenWidth();

void VGA_setChar(uint16_t pos, uint16_t chr);
uint16_t VGA_getChar(uint16_t pos);

void VGA_setCursor(uint16_t pos);
