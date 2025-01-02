#pragma once
#include <stdint.h>
#include <stdbool.h>

#define TERMINAL_COLOR_BLACK 0
#define TERMINAL_COLOR_BLUE 4
#define TERMINAL_COLOR_GREEN 2
#define TERMINAL_COLOR_CYAN 6
#define TERMINAL_COLOR_RED 1
#define TERMINAL_COLOR_MAGENTA 5
#define TERMINAL_COLOR_BROWN 3
#define TERMINAL_COLOR_LIGHT_GREY 7
#define TERMINAL_COLOR_DARK_GREY 8
#define TERMINAL_COLOR_LIGHT_BLUE 12
#define TERMINAL_COLOR_LIGHT_GREEN 10
#define TERMINAL_COLOR_LIGHT_CYAN 14
#define TERMINAL_COLOR_LIGHT_RED 9
#define TERMINAL_COLOR_LIGHT_MAGENTA 13
#define TERMINAL_COLOR_LIGHT_BROWN 11
#define TERMINAL_COLOR_WHITE 15

// Escape Sequenz for colorization
#define TS(x) #x
#define Color(fg, bg) "\033[38;5;" TS(fg) ";48;5;" TS(bg) "m"

typedef struct {
    const char* name;
    bool (*check)(void);
    void (*activate)(void);
    void (*deactivate)(void);

    // ASCII (Byte Stream) Driver
    void (*putc)(char c);
    void (*clear)(void);
} display_driver;

void terminal_init();
void terminal_putc(char c);
void terminal_puts(const char* str);
void terminal_clear();
