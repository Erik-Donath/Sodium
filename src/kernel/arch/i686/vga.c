#include "vga.h"

static uint16_t* ScreenBuffer = (uint16_t*)VGA_SCREEN_BUFFER_ADDRESS;
static uint8_t cursorX = 0, cursorY = 0;
static uint8_t currentColor = VGA_DEFAULT_COLOR;

void VGA_setchr(uint8_t x, uint8_t y, char c, uint8_t color) {
    ScreenBuffer[y * VGA_SCREEN_WIDTH + x] = (color << 8) | c;
}

char VGA_getchr(uint8_t x, uint8_t y) {
    return (char)(ScreenBuffer[y * VGA_SCREEN_WIDTH + x] & 0x00FF);
}
uint8_t VGA_getcolor(uint8_t x, uint8_t y) {
    return (uint8_t)((ScreenBuffer[y * VGA_SCREEN_WIDTH + x] >> 8) & 0x00FF);
}

void VGA_clrscr() {
    for(uint8_t y = 0; y < VGA_SCREEN_HEIGHT; y++) {
        for(uint8_t x = 0; x < VGA_SCREEN_WIDTH; x++) {
            VGA_setchr(x, y, '\0', currentColor);
        }
    }
}

void VGA_scrollback(uint8_t lines) {
    for (uint8_t y = lines; y < VGA_SCREEN_HEIGHT; y++) {
        for (uint8_t x = 0; x < VGA_SCREEN_WIDTH; x++) {
            VGA_setchr(x, y -lines, VGA_getchr(x, y), VGA_getcolor(x, y));
        }
    }

    for (uint8_t y = VGA_SCREEN_HEIGHT - lines; y < VGA_SCREEN_HEIGHT; y++) {
        for (uint8_t x = 0; x < VGA_SCREEN_WIDTH; x++) {
            VGA_setchr(x, y, '\0', currentColor);
        }
    }
}

void VGA_setcursor(uint8_t x, uint8_t y) {
    cursorX = x;
    cursorY = y;
    
    /*
    uint16_t pos = y * VGA_SCREEN_WIDTH + x;
    i686_outb(0x3D4, 0x0F);
    i686_outb(0x3D5, (uint8_t)(pos & 0xFF));
    i686_outb(0x3D4, 0x0E);
    i686_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
    */
}

void VGA_advancecursor() {
    cursorX++;

    if (cursorX >= VGA_SCREEN_WIDTH) {
        cursorX = 0;
        cursorY++;
    }
    if (cursorY >= VGA_SCREEN_HEIGHT) {
        VGA_scrollback(1);
        cursorY = VGA_SCREEN_HEIGHT - 1;
    }

    VGA_setcursor(cursorX, cursorY);
}

void VGA_putchr(char c, uint8_t color) {
    VGA_setchr(cursorX, cursorY, c, color);
    VGA_advancecursor();
}

void VGA_putc(char c) {
    switch(c) {
        case '\n':
            VGA_setcursor(0, cursorY + 1);
            break;
        case '\t':
            uint8_t tabSteps = 4 - (cursorX % 4);
            for (int i = 0; i < tabSteps; i++)
                VGA_putchr(' ', currentColor);
            break;
        case '\r':
            VGA_setcursor(0, cursorY);
            break;
        default:
            VGA_putchr(c, currentColor);
            break;
    }
}

void VGA_puts(const char* str) {
    while(*str) {
        VGA_putc(*str);
        str++;
    }
}

void VGA_setcolor(uint8_t color) {
    currentColor = color;
}
uint8_t VGA_getcurcolor() {
    return currentColor;
}
uint8_t VGA_getcursorx() {
    return cursorX;
}
uint8_t VGA_getcursory() {
    return cursorY;
}
