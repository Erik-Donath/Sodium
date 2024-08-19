#include <kernel/arch/i686/vga.h>
#include "display.h"

static color    DISPLAY_color = 0;

static uint8_t  DISPLAY_Screen_Height = 0,
                DISPLAY_Screen_Width  = 0;

static uint8_t  DISPLAY_Cursor_X = 0,
                DISPLAY_Cursor_Y = 0;

#define CALC_Pos(x, y) (uint16_t)(y * DISPLAY_Screen_Width + x)

void DISPLAY_init() {
    VGA_init();

    DISPLAY_Screen_Height = VGA_getScreenHeight();
    DISPLAY_Screen_Width  = VGA_getScreenWidth();

    DISPLAY_color = DEFAULT_COLOR;
    DISPLAY_clear();
}

void DISPLAY_clear() {
    for(uint8_t y = 0; y < DISPLAY_Screen_Height; y++) {
        for(uint8_t x = 0; x < DISPLAY_Screen_Width; x++) {
            DISPLAY_setc(x, y, CCHR('\0', DISPLAY_color));
        }
    }
    DISPLAY_setcursor(0, 0);
}

void DISPLAY_setcolor(color c) {
    DISPLAY_color = c;
}

color DISPLAY_getcolor() {
    return DISPLAY_color;
}

void DISPLAY_nextc(char c) {
    DISPLAY_setc(DISPLAY_Cursor_X, DISPLAY_Cursor_Y, CCHR(c, DISPLAY_color));

    // Advance Cursor by 1 Step
    uint8_t x = DISPLAY_Cursor_X,
            y = DISPLAY_Cursor_Y;
    
    x++;
    if(x >= DISPLAY_Screen_Width) {
        x = 0;
        y++;
    }
    if(y >= DISPLAY_Screen_Height) {
        y = DISPLAY_Screen_Height - 1;
        DISPLAY_scroll(1);
    }

    DISPLAY_setcursor(x, y);
}


void DISPLAY_putc(char c) {
    switch(c) {
        case '\n':
            DISPLAY_setcursor(0, DISPLAY_Cursor_Y + 1);
            break;
        case '\t':
            uint8_t tabSteps = 4 - (DISPLAY_Cursor_X % 4);
            for (int i = 0; i < tabSteps; i++)
                DISPLAY_nextc(' ');
            break;
        case '\r':
            DISPLAY_setcursor(0, DISPLAY_Cursor_Y);
            break;
        default:
            DISPLAY_nextc(c);
            break;
    }
}

void DISPLAY_setc(uint8_t x, uint8_t y, cchr c) {
    if(x >= DISPLAY_Screen_Width || y >= DISPLAY_Screen_Height) return;
    VGA_setChar(CALC_Pos(x, y), (uint16_t)c);
}

cchr DISPLAY_getc(uint8_t x, uint8_t y) {
    if(x >= DISPLAY_Screen_Width || y >= DISPLAY_Screen_Height) return 0x0000;
    return VGA_getChar(CALC_Pos(x, y));
}

void DISPLAY_setcursor(uint8_t x, uint8_t y) {
    if(x >= DISPLAY_Screen_Width || y >= DISPLAY_Screen_Height) {
        DISPLAY_Cursor_X = DISPLAY_Screen_Width  - 1;
        DISPLAY_Cursor_Y = DISPLAY_Screen_Height - 1;
    }
    else {
        DISPLAY_Cursor_X = x;
        DISPLAY_Cursor_Y = y;
    }
    VGA_setCursor(CALC_Pos(x, y));
}

void DISPLAY_scroll(uint8_t lines) {
    for (uint8_t y = lines; y < DISPLAY_Screen_Height; y++) {
        for (uint8_t x = 0; x < DISPLAY_Screen_Width; x++) {
            DISPLAY_setc(x, y - lines, DISPLAY_getc(x, y));
        }
    }

    for (uint8_t y = DISPLAY_Screen_Height - lines; y < DISPLAY_Screen_Height; y++) {
        for (uint8_t x = 0; x < DISPLAY_Screen_Width; x++) {
            DISPLAY_setc(x, y, CCHR('\0', DISPLAY_color));
        }
    }
}

uint8_t DISPLAY_getDisplayHeight() {
    return DISPLAY_Screen_Height;
}

uint8_t DISPLAY_getDisplayWidth() {
    return DISPLAY_Screen_Width;
}