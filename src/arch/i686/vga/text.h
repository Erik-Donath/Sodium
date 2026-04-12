#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "vga.h"

typedef enum i686_vga_color : uint8_t {
    i686_VGA_COLOR_BLACK         = 0x0,
    i686_VGA_COLOR_BLUE          = 0x1,
    i686_VGA_COLOR_GREEN         = 0x2,
    i686_VGA_COLOR_CYAN          = 0x3,
    i686_VGA_COLOR_RED           = 0x4,
    i686_VGA_COLOR_MAGENTA       = 0x5,
    i686_VGA_COLOR_BROWN         = 0x6,
    i686_VGA_COLOR_LIGHT_GREY    = 0x7,
    i686_VGA_COLOR_DARK_GREY     = 0x8,
    i686_VGA_COLOR_LIGHT_BLUE    = 0x9,
    i686_VGA_COLOR_LIGHT_GREEN   = 0xA,
    i686_VGA_COLOR_LIGHT_CYAN    = 0xB,
    i686_VGA_COLOR_LIGHT_RED     = 0xC,
    i686_VGA_COLOR_LIGHT_MAGENTA = 0xD,
    i686_VGA_COLOR_YELLOW        = 0xE,
    i686_VGA_COLOR_WHITE         = 0xF,
} i686_vga_color_t;

#define i686_vga_text_make_attr(fg, bg) (uint8_t)(((bg) << 4) | ((fg) & 0x0F))
#define i686_vga_text_make_cell(c, attr) (uint16_t)(((attr) << 8) | ((c) & 0xFF))
#define i686_vga_text_make_char(c, fg, bg) (uint16_t)((i686_vga_text_make_attr(fg, bg) << 8) | ((c) & 0xFF))

typedef struct i686_vga_text_mode {
    uint8_t cols;
    uint8_t rows;
    uint8_t font_height;
    bool blink_enabled;
} i686_vga_text_mode_t;

#define i686_VGA_TEXT_MODE_80x25 ((i686_vga_text_mode_t){ 80, 25, 16, false })
#define i686_VGA_TEXT_MODE_80x50 ((i686_vga_text_mode_t){ 80, 50,  8, false })
#define i686_VGA_TEXT_MODE_40x25 ((i686_vga_text_mode_t){ 40, 25, 16, false })

#define i686_VGA_TEXT_MODE_MAX_COLS 80
#define i686_VGA_TEXT_MODE_MAX_ROWS 50

void i686_vga_text_init(i686_vga_text_mode_t mode);
i686_vga_text_mode_t vga_text_get_mode(void);

void i686_vga_text_put_cell(uint8_t x, uint8_t y, uint16_t cell);
uint16_t i686_vga_text_get_cell(uint8_t x, uint8_t y);
void i686_vga_text_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t cell);
void i686_vga_text_clear(uint8_t attr);

void i686_vga_text_scroll_up(uint8_t lines, uint8_t fill_attr);
void i686_vga_text_scroll_down(uint8_t lines, uint8_t fill_attr);

void i686_vga_text_cursor_set_pos(uint8_t x, uint8_t y);
void i686_vga_text_cursor_get_pos(uint8_t *x, uint8_t *y);
void i686_vga_text_cursor_set_shape(uint8_t scan_start, uint8_t scan_end);
void i686_vga_text_cursor_set_visible(bool visible);

void i686_vga_text_set_blink(bool enabled);
void i686_vga_text_set_palette_entry(uint8_t slot, uint8_t ega_color);
void i686_vga_text_set_palette(uint8_t ega_colors[16]);

void i686_vga_text_load_font(uint8_t slot, const uint8_t *data, uint16_t glyph_count, uint8_t font_height);
