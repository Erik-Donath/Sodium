#include "text.h"

static volatile uint16_t* const vga_buf = (volatile uint16_t*)i686_VGA_TEXT_BASE;
static uint16_t vga_shadow[i686_VGA_TEXT_MODE_MAX_COLS * i686_VGA_TEXT_MODE_MAX_ROWS];
static i686_vga_text_mode_t vga_mode;

#define calc_idx(x, y) ((y) * vga_mode.cols + (x))
#define put_cell(idx, cell) (vga_buf[(idx)] = vga_shadow[(idx)] = (cell))
#define get_cell(idx) (vga_shadow[(idx)])

void i686_vga_text_init(i686_vga_text_mode_t mode) {
    vga_mode = mode;
}

i686_vga_text_mode_t vga_text_get_mode(void) {
    return vga_mode;
}

void i686_vga_text_put_cell(uint8_t x, uint8_t y, uint16_t cell) {
    if(x > vga_mode.rows || y > vga_mode.cols)
        return; // #FIXME: Might want to return boolean
    uint16_t idx = calc_idx(x, y);
    put_cell(idx, cell);
}

uint16_t i686_vga_text_get_cell(uint8_t x, uint8_t y) {
    if(x > vga_mode.rows || y > vga_mode.cols)
        return 0x0000;
    return get_cell(calc_idx(x, y));
}

void i686_vga_text_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t cell) {
    for(uint8_t iy = y; iy < (y+h); iy++) {
        for(uint8_t ix = x; ix < (x+w); ix++) {
            i686_vga_text_put_cell(ix, iy, cell);
        }
    }
}

void i686_vga_text_clear(uint8_t attr) {
    uint16_t cell = i686_vga_text_make_cell('\0', attr);
    uint16_t total = vga_mode.cols * vga_mode.rows;
    for(uint16_t i = 0; i < total; i++) {
        put_cell(i, cell);
    }
}

void i686_vga_text_scroll_up(uint8_t lines, uint8_t fill_attr) {
    (void)lines;
    (void)fill_attr;
}

void i686_vga_text_scroll_down(uint8_t lines, uint8_t fill_attr) {
    (void)lines;
    (void)fill_attr;
}

void i686_vga_text_cursor_set_pos(uint8_t x, uint8_t y) {
    (void)x;
    (void)y;
}

void i686_vga_text_cursor_get_pos(uint8_t *x, uint8_t *y) {
    (void)x;
    (void)y;
}

void i686_vga_text_cursor_set_shape(uint8_t scan_start, uint8_t scan_end) {
    (void)scan_start;
    (void)scan_end;
}

void i686_vga_text_cursor_set_visible(bool visible) {
    (void)visible;
}

void i686_vga_text_set_blink(bool enabled) {
    (void)enabled;
}

void i686_vga_text_set_palette_entry(uint8_t slot, uint8_t ega_color) {
    (void)slot;
    (void)ega_color;
}

void i686_vga_text_set_palette(uint8_t ega_colors[16]) {
    (void)ega_colors;
}

void i686_vga_text_load_font(uint8_t slot, const uint8_t *data, uint16_t glyph_count, uint8_t font_height) {
    (void)slot;
    (void)data;
    (void)glyph_count;
    (void)font_height;
}
