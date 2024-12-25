#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    TERMINAL_COLOR_BLACK         = 0x0,
    TERMINAL_COLOR_BLUE          = 0x1,
    TERMINAL_COLOR_GREEN         = 0x2,
    TERMINAL_COLOR_CYAN          = 0x3,
    TERMINAL_COLOR_RED           = 0x4,
    TERMINAL_COLOR_MAGENTA       = 0x5,
    TERMINAL_COLOR_BROWN         = 0x6,
    TERMINAL_COLOR_LIGHT_GREY    = 0x7,
    TERMINAL_COLOR_DARK_GREY     = 0x8,
    TERMINAL_COLOR_LIGHT_BLUE    = 0x9,
    TERMINAL_COLOR_LIGHT_GREEN   = 0xA,
    TERMINAL_COLOR_LIGHT_CYAN    = 0xB,
    TERMINAL_COLOR_LIGHT_RED     = 0xC,
    TERMINAL_COLOR_LIGHT_MAGENTA = 0xD,
    TERMINAL_COLOR_LIGHT_BROWN   = 0xE,
    TERMINAL_COLOR_WHITE         = 0xF,
} terminal_color_types;

#define Color(fg, bg) fg | bg << 4

typedef struct {
    const char* name;

    void (*init)(void);
    bool (*check)(void);
    
    // Display Driver is here just a byte stream in Textmode + 8bit color (4 fb, 4 bg)
    void (*put_char)(char c, uint8_t color);
    void (*new_line)(void);
    void (*scroll)(uint8_t lines);
    void (*clear)(void);
} display_driver;

void terminal_init();
void terminal_putchar(char c);
void terminal_write(const char* str);
void terminal_set_color(terminal_color_types fg, terminal_color_types bg);
void terminal_scroll(uint8_t lines);
void terminal_clear();
void terminal_status();
