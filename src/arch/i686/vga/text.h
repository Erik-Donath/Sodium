#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum i686_vga_color : uint8_t {
    I686_VGA_COLOR_BLACK         = 0x0,
    I686_VGA_COLOR_BLUE          = 0x1,
    I686_VGA_COLOR_GREEN         = 0x2,
    I686_VGA_COLOR_CYAN          = 0x3,
    I686_VGA_COLOR_RED           = 0x4,
    I686_VGA_COLOR_MAGENTA       = 0x5,
    I686_VGA_COLOR_BROWN         = 0x6,
    I686_VGA_COLOR_LIGHT_GREY    = 0x7,
    I686_VGA_COLOR_DARK_GREY     = 0x8,
    I686_VGA_COLOR_LIGHT_BLUE    = 0x9,
    I686_VGA_COLOR_LIGHT_GREEN   = 0xA,
    I686_VGA_COLOR_LIGHT_CYAN    = 0xB,
    I686_VGA_COLOR_LIGHT_RED     = 0xC,
    I686_VGA_COLOR_LIGHT_MAGENTA = 0xD,
    I686_VGA_COLOR_YELLOW        = 0xE,
    I686_VGA_COLOR_WHITE         = 0xF,
} i686_vga_color_t;

// @brief Pack foreground and background colour into one attribute byte.
#define I686_VGA_TEXT_MAKE_ATTR(fg, bg)     (uint8_t)(((bg) << 4) | ((fg) & 0x0F))

// @brief Pack character + attribute into a 16-bit VGA cell.
#define I686_VGA_TEXT_MAKE_CELL(c, attr)    (uint16_t)(((attr) << 8) | ((c) & 0xFF))

// @brief Pack character + fg/bg colours into a VGA cell directly.
#define I686_VGA_TEXT_MAKE_CHAR(c, fg, bg)  I686_VGA_TEXT_MAKE_CELL((c), I686_VGA_TEXT_MAKE_ATTR((fg), (bg)))

// Keep the old lowercase macros as aliases to avoid breaking call sites.
#define i686_vga_text_make_attr  I686_VGA_TEXT_MAKE_ATTR
#define i686_vga_text_make_cell  I686_VGA_TEXT_MAKE_CELL
#define i686_vga_text_make_char  I686_VGA_TEXT_MAKE_CHAR

typedef struct i686_vga_text_mode {
    uint8_t cols;
    uint8_t rows;
    uint8_t font_height;
    bool    blink_enabled;
} i686_vga_text_mode_t;

// WARNING: Do not use I686_VGA_TEXT_MODE_80x50: it has a register timing
//          bug that may damage older CRT monitors.
#define I686_VGA_TEXT_MODE_80x25  ((i686_vga_text_mode_t){ 80, 25, 16, false })
#define I686_VGA_TEXT_MODE_80x50  ((i686_vga_text_mode_t){ 80, 50,  8, false })
#define I686_VGA_TEXT_MODE_40x25  ((i686_vga_text_mode_t){ 40, 25, 16, false })

#define I686_VGA_TEXT_MODE_MAX_COLS  80
#define I686_VGA_TEXT_MODE_MAX_ROWS  50

// @brief Initialise the VGA controller for the requested text mode.
// @param mode  Use one of the I686_VGA_TEXT_MODE_* constants.
void i686_vga_text_init(i686_vga_text_mode_t mode);

// @brief Return the currently active text mode.
i686_vga_text_mode_t i686_vga_text_get_mode(void);

// @brief Write a cell (character + attribute) at (x, y). OOB writes are ignored.
void i686_vga_text_put_cell(uint8_t x, uint8_t y, uint16_t cell);

// @brief Read the cell at (x, y) from the shadow buffer. Returns 0 if OOB.
uint16_t i686_vga_text_get_cell(uint8_t x, uint8_t y);

// @brief Fill a rectangular region with a single cell value.
void i686_vga_text_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t cell);

// @brief Clear the screen; fill every cell with space + attr.
void i686_vga_text_clear(uint8_t attr);

// @brief Scroll up by lines rows. New rows at the bottom are blank (fill_attr).
void i686_vga_text_scroll_up(uint8_t lines, uint8_t fill_attr);

// @brief Scroll down by lines rows. New rows at the top are blank.
void i686_vga_text_scroll_down(uint8_t lines, uint8_t fill_attr);

// @brief Move the hardware cursor to column x, row y.
void i686_vga_text_cursor_set_pos(uint8_t x, uint8_t y);

// @brief Read the hardware cursor position.
void i686_vga_text_cursor_get_pos(uint8_t *x, uint8_t *y);

// @brief Set cursor shape: scan_start = first scan line, scan_end = last.
void i686_vga_text_cursor_set_shape(uint8_t scan_start, uint8_t scan_end);

// @brief Read the current cursor shape scan lines.
void i686_vga_text_cursor_get_shape(uint8_t *scan_start, uint8_t *scan_end);

// @brief Show or hide the hardware cursor.
void i686_vga_text_cursor_set_visible(bool visible);

// @brief Enable/disable character blinking.
//        Disabled: fg attribute bit 3 selects from 16 background colours.
void i686_vga_text_set_blink(bool enabled);

// @brief Set one EGA palette slot (0–15) to the given 6-bit EGA colour index.
void i686_vga_text_set_palette_entry(uint8_t slot, uint8_t ega_color);

// @brief Replace all 16 palette entries atomically.
void i686_vga_text_set_palette(const uint8_t ega_colors[16]);

// @brief Read one palette entry from the shadow copy. Returns 0 if OOB.
uint8_t i686_vga_text_get_palette_entry(uint8_t slot);

// @brief Copy all 16 palette entries from the shadow copy into out.
void i686_vga_text_get_palette(uint8_t out[16]);

// @brief Select font slot for character maps A and B (0 or 1 each).
void i686_vga_text_set_char_map(uint8_t slot_a, uint8_t slot_b);

// @brief Load raw font bitmap into VGA plane 2.
// @param slot         Font slot (0 or 1).
// @param data         Packed bitmap: glyph_count × font_height bytes.
// @param glyph_count  Number of glyphs (1–256).
// @param font_height  Scan lines per glyph (8 or 16).
void i686_vga_text_load_font(uint8_t slot, const uint8_t *data, uint16_t glyph_count, uint8_t font_height);
