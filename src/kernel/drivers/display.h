#pragma once
#include <stdint.h>
#include <stdbool.h>

// Display driver interface - Hardware Abstraction Layer
// This defines the generic interface that all display drivers must implement

typedef struct {
    const char* name;
    bool (*check)(void);
    void (*activate)(void);
    void (*deactivate)(void);

    // ASCII (Byte Stream) Driver
    void (*putc)(char c);
    void (*clear)(void);
} display_driver_t;

// Color definitions for display drivers
#define DISPLAY_COLOR_BLACK 0
#define DISPLAY_COLOR_BLUE 4
#define DISPLAY_COLOR_GREEN 2
#define DISPLAY_COLOR_CYAN 6
#define DISPLAY_COLOR_RED 1
#define DISPLAY_COLOR_MAGENTA 5
#define DISPLAY_COLOR_BROWN 3
#define DISPLAY_COLOR_LIGHT_GREY 7
#define DISPLAY_COLOR_DARK_GREY 8
#define DISPLAY_COLOR_LIGHT_BLUE 12
#define DISPLAY_COLOR_LIGHT_GREEN 10
#define DISPLAY_COLOR_LIGHT_CYAN 14
#define DISPLAY_COLOR_LIGHT_RED 9
#define DISPLAY_COLOR_LIGHT_MAGENTA 13
#define DISPLAY_COLOR_LIGHT_BROWN 11
#define DISPLAY_COLOR_WHITE 15

// Escape sequence for colorization
#define TS(x) #x
#define Color(fg, bg) "\033[38;5;" TS(fg) ";48;5;" TS(bg) "m"
