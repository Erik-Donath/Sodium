#include "text.h"
#include "vga.h"
#include "../cpu/io.h"
#include <string.h>

static volatile uint16_t* const vga_buf = (volatile uint16_t*)i686_VGA_TEXT_BASE;
static uint16_t vga_shadow[i686_VGA_TEXT_MODE_MAX_COLS * i686_VGA_TEXT_MODE_MAX_ROWS];
static uint8_t  palette_shadow[16];
static i686_vga_text_mode_t vga_mode;

#define COLS  (vga_mode.cols)
#define ROWS  (vga_mode.rows)
#define TOTAL ((uint16_t)(ROWS * COLS))

#define calc_idx(x, y)      ((uint16_t)((y) * COLS + (x)))
#define put_cell(idx, cell) (vga_buf[(idx)] = vga_shadow[(idx)] = (cell))
#define get_cell(idx)       (vga_shadow[(idx)])

#define EMPTY_CHAR ((char)' ')

static const uint8_t crtc_regs_80x25[25] = {
//  H_TOT  H_END  HBS    HBE    HSS    HSE    V_TOT  OFLOW
    0x5F,  0x4F,  0x50,  0x82,  0x55,  0x81,  0xBF,  0x1F,
//  PRE    MSCLN  CUR_S  CUR_E  ST_HI  ST_LO  CH_HI  CH_LO
    0x00,  0x4F,  0x0D,  0x0E,  0x00,  0x00,  0x00,  0x00,
//  VSS    VSE    V_END  OFFST  UNDLN  VBS    VBE    MODE
    0x9C,  0x8E,  0x8F,  0x28,  0x1F,  0x96,  0xB9,  0xA3,
//  LCMP
    0xFF
};

// #FIXME: These Register Values result in a graphics glitch where only the top part of the char is printed. Might be a font problem too???
static const uint8_t crtc_regs_80x50[25] = {
//  H_TOT  H_END  HBS    HBE    HSS    HSE    V_TOT  OFLOW
    0x5F,  0x4F,  0x50,  0x82,  0x55,  0x81,  0xBF,  0x1F,
//  PRE    MSCLN  CUR_S  CUR_E  ST_HI  ST_LO  CH_HI  CH_LO
    0x00,  0x07,  0xea,  0x10, 0x00,  0x00,  0x00,  0x00,
//  VSS    VSE    V_END  OFFST  UNDLN  VBS    VBE    MODE
    0x9C,  0x8E,  0x8F,  0x28,  0x1F,  0x96,  0xB9,  0xA3,
//  LCMP
    0xFF
};

static const uint8_t crtc_regs_40x25[25] = {
//  H_TOT  H_END  HBS    HBE    HSS    HSE    V_TOT  OFLOW
    0x2D,  0x27,  0x28,  0x90,  0x2B,  0xA0,  0xBF,  0x1F,
//  PRE    MSCLN  CUR_S  CUR_E  ST_HI  ST_LO  CH_HI  CH_LO
    0x00,  0x4F,  0x0D,  0x0E,  0x00,  0x00,  0x00,  0x00,
//  VSS    VSE    V_END  OFFST  UNDLN  VBS    VBE    MODE
    0x9C,  0x8E,  0x8F,  0x14,  0x1F,  0x96,  0xB9,  0xA3,
//  LCMP
    0xFF
};

static const uint8_t seq_regs_text[4] = {
    0x00,  // 0x01 Clocking Mode:  8-dot character clock
    0x03,  // 0x02 Map Mask:       write to planes 0 and 1 (text data)
    0x00,  // 0x03 Char Map Sel:   both maps → slot 0; use set_char_map() to change
    0x02,  // 0x04 Memory Mode:    extended memory, odd/even enabled
};

static const uint8_t gc_regs_text[9] = {
    0x00,  // 0x00 Set/Reset
    0x00,  // 0x01 Enable Set/Reset
    0x00,  // 0x02 Color Compare
    0x00,  // 0x03 Data Rotate
    0x00,  // 0x04 Read Map Select
    0x10,  // 0x05 Mode:  odd/even read, write mode 0
    0x0E,  // 0x06 Misc:  text mode map (bits[3:2]=11 → 0xB8000), chain odd/even
    0x00,  // 0x07 Color Don't Care
    0xFF,  // 0x08 Bit Mask
};

static const uint8_t ac_default_palette[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};

static void vga_sync_all(void) {
    for (uint16_t i = 0; i < TOTAL; i++)
        vga_buf[i] = vga_shadow[i];
}

void i686_vga_text_init(i686_vga_text_mode_t mode) {
    vga_mode = mode;

    const uint8_t *crtc;
    if (mode.cols == 40)
        crtc = crtc_regs_40x25;
    else if (mode.rows == 50)
        crtc = crtc_regs_80x50;
    else
        crtc = crtc_regs_80x25;

    // 1. Miscellaneous Output: color mode, RAM enabled, 28 MHz clock, HSYNC–
    i686_io_outb(i686_VGA_PORT_MISC_WRITE,
        i686_VGA_MISC_IO_COLOR   |
        i686_VGA_MISC_RAM_ENABLE |
        i686_VGA_MISC_CLK_28MHZ  |
        i686_VGA_MISC_ODD_PAGE   |
        i686_VGA_MISC_HSYNC_NEG);

    // 2. Sequencer: enter synchronous reset, program, exit reset
    i686_vga_seq_write(i686_VGA_SEQ_RESET, i686_VGA_SEQ_RESET_SYNC);
    for (uint8_t i = 0; i < 4; i++)
        i686_vga_seq_write(i + 1, seq_regs_text[i]);
    i686_vga_seq_write(i686_VGA_SEQ_RESET, i686_VGA_SEQ_RESET_CLEAR);

    // 3. CRTC: clear write-protect on registers 0x00–0x07 before programming
    i686_vga_crtc_write(i686_VGA_CRTC_V_SYNC_END,
        i686_vga_crtc_read(i686_VGA_CRTC_V_SYNC_END) & ~i686_VGA_CRTC_PROTECT_BIT);
    for (uint8_t i = 0; i < 25; i++)
        i686_vga_crtc_write(i, crtc[i]);

    // 4. Graphics Controller
    for (uint8_t i = 0; i < 9; i++)
        i686_vga_gc_write(i, gc_regs_text[i]);

    // 5. Attribute Controller: reset flip-flop, write palette, then mode regs
    i686_vga_ac_reset_flipflop();
    for (uint8_t i = 0; i < 16; i++) {
        i686_vga_ac_write(i, ac_default_palette[i]);
        palette_shadow[i] = ac_default_palette[i];
    }

    i686_vga_ac_write(i686_VGA_AC_MODE_CTRL,
        mode.blink_enabled ? i686_VGA_AC_MODE_BLINK_ENABLE : 0x00);
    i686_vga_ac_write(i686_VGA_AC_OVERSCAN,       0x00);  // black border
    i686_vga_ac_write(i686_VGA_AC_COLOR_PLANE_EN, 0x0F);  // all planes visible
    i686_vga_ac_write(i686_VGA_AC_H_PAN,          0x08);  // no pixel panning
    i686_vga_ac_write(i686_VGA_AC_COLOR_SELECT,   0x00);

    // Re-enable palette output; leaves flip-flop in a defined state
    i686_io_outb(i686_VGA_PORT_AC_INDEX, i686_VGA_AC_PALETTE_ENABLE);

    i686_vga_text_clear(i686_vga_text_make_attr(
        i686_VGA_COLOR_LIGHT_GREY, i686_VGA_COLOR_BLACK));
}

i686_vga_text_mode_t i686_vga_text_get_mode(void) {
    return vga_mode;
}

void i686_vga_text_put_cell(uint8_t x, uint8_t y, uint16_t cell) {
    if (x >= COLS || y >= ROWS)
        return;
    put_cell(calc_idx(x, y), cell);
}

uint16_t i686_vga_text_get_cell(uint8_t x, uint8_t y) {
    if (x >= COLS || y >= ROWS)
        return 0x0000;
    return get_cell(calc_idx(x, y));
}

void i686_vga_text_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint16_t cell) {
    for (uint8_t iy = y; iy < (uint8_t)(y + h); iy++)
        for (uint8_t ix = x; ix < (uint8_t)(x + w); ix++)
            i686_vga_text_put_cell(ix, iy, cell);
}

void i686_vga_text_clear(uint8_t attr) {
    uint16_t cell = i686_vga_text_make_cell(EMPTY_CHAR, attr);
    for (uint16_t i = 0; i < TOTAL; i++)
        vga_shadow[i] = cell;
    vga_sync_all();
}

void i686_vga_text_scroll_up(uint8_t lines, uint8_t fill_attr) {
    if (lines == 0)
        return;
    if (lines >= ROWS) {
        i686_vga_text_clear(fill_attr);
        return;
    }

    uint16_t shift = (uint16_t)(lines * COLS);
    uint16_t fill  = i686_vga_text_make_cell(EMPTY_CHAR, fill_attr);

    memmove(vga_shadow, vga_shadow + shift,
            (size_t)(TOTAL - shift) * sizeof(uint16_t));
    for (uint16_t i = (uint16_t)(TOTAL - shift); i < TOTAL; i++)
        vga_shadow[i] = fill;

    vga_sync_all();
}

void i686_vga_text_scroll_down(uint8_t lines, uint8_t fill_attr) {
    if (lines == 0)
        return;
    if (lines >= ROWS) {
        i686_vga_text_clear(fill_attr);
        return;
    }

    uint16_t shift = (uint16_t)(lines * COLS);
    uint16_t fill  = i686_vga_text_make_cell(EMPTY_CHAR, fill_attr);

    memmove(vga_shadow + shift, vga_shadow,
            (size_t)(TOTAL - shift) * sizeof(uint16_t));
    for (uint16_t i = 0; i < shift; i++)
        vga_shadow[i] = fill;

    vga_sync_all();
}

void i686_vga_text_cursor_set_pos(uint8_t x, uint8_t y) {
    uint16_t pos = calc_idx(x, y);
    i686_vga_crtc_write(i686_VGA_CRTC_CURSOR_LO, (uint8_t)(pos & 0xFF));
    i686_vga_crtc_write(i686_VGA_CRTC_CURSOR_HI, (uint8_t)(pos >> 8));
}

void i686_vga_text_cursor_get_pos(uint8_t *x, uint8_t *y) {
    uint16_t pos = (uint16_t)i686_vga_crtc_read(i686_VGA_CRTC_CURSOR_LO)
                 | (uint16_t)(i686_vga_crtc_read(i686_VGA_CRTC_CURSOR_HI) << 8);
    *x = (uint8_t)(pos % COLS);
    *y = (uint8_t)(pos / COLS);
}

void i686_vga_text_cursor_set_shape(uint8_t scan_start, uint8_t scan_end) {
    i686_vga_crtc_write(i686_VGA_CRTC_CURSOR_START,
        scan_start & (uint8_t)~i686_VGA_CURSOR_DISABLE_BIT);
    i686_vga_crtc_write(i686_VGA_CRTC_CURSOR_END, scan_end);
}

void i686_vga_text_cursor_get_shape(uint8_t *scan_start, uint8_t *scan_end) {
    *scan_start = i686_vga_crtc_read(i686_VGA_CRTC_CURSOR_START)
                & (uint8_t)~i686_VGA_CURSOR_DISABLE_BIT;
    *scan_end   = i686_vga_crtc_read(i686_VGA_CRTC_CURSOR_END);
}

void i686_vga_text_cursor_set_visible(bool visible) {
    uint8_t val = i686_vga_crtc_read(i686_VGA_CRTC_CURSOR_START);
    if (visible)
        val &= (uint8_t)~i686_VGA_CURSOR_DISABLE_BIT;
    else
        val |= i686_VGA_CURSOR_DISABLE_BIT;
    i686_vga_crtc_write(i686_VGA_CRTC_CURSOR_START, val);
}

void i686_vga_text_set_blink(bool enabled) {
    vga_mode.blink_enabled = enabled;
    i686_vga_ac_reset_flipflop();
    i686_vga_ac_write(i686_VGA_AC_MODE_CTRL,
        enabled ? i686_VGA_AC_MODE_BLINK_ENABLE : 0x00);
    i686_io_outb(i686_VGA_PORT_AC_INDEX, i686_VGA_AC_PALETTE_ENABLE);
}

void i686_vga_text_set_palette_entry(uint8_t slot, uint8_t ega_color) {
    if (slot > 0x0F)
        return;
    palette_shadow[slot] = ega_color;
    i686_vga_ac_reset_flipflop();
    i686_vga_ac_write(slot, ega_color);
    i686_io_outb(i686_VGA_PORT_AC_INDEX, i686_VGA_AC_PALETTE_ENABLE);
}

void i686_vga_text_set_palette(const uint8_t ega_colors[16]) {
    memcpy(palette_shadow, ega_colors, 16);
    i686_vga_ac_reset_flipflop();
    for (uint8_t i = 0; i < 16; i++)
        i686_vga_ac_write(i, ega_colors[i]);
    i686_io_outb(i686_VGA_PORT_AC_INDEX, i686_VGA_AC_PALETTE_ENABLE);
}

uint8_t i686_vga_text_get_palette_entry(uint8_t slot) {
    if (slot > 0x0F)
        return 0x00;
    return palette_shadow[slot];
}

void i686_vga_text_get_palette(uint8_t out[16]) {
    memcpy(out, palette_shadow, 16);
}

void i686_vga_text_set_char_map(uint8_t slot_a, uint8_t slot_b) {
    if (slot_a > 1 || slot_b > 1)
        return;
    // SEQ_CHAR_MAP encoding: bits[3:2] = map A index, bits[1:0] = map B index.
    // Index 0 -> plane-2 offset 0x0000 (slot 0), index 1 -> 0x4000 (slot 1).
    i686_vga_seq_write(i686_VGA_SEQ_CHAR_MAP,
        (uint8_t)((slot_a << 2) | slot_b));
}

void i686_vga_text_load_font(uint8_t slot, const uint8_t *data,
                              uint16_t glyph_count, uint8_t font_height) {
    if (slot > 1 || glyph_count == 0 || glyph_count > 256)
        return;

    volatile uint8_t *font_plane = (volatile uint8_t *)i686_VGA_GRAPH_BASE;
    uint32_t slot_offset = (slot == 0)
        ? i686_VGA_FONT_SLOT_0_OFFSET
        : i686_VGA_FONT_SLOT_1_OFFSET;

    // Switch sequencer to write plane 2 only; disable odd/even interleave
    i686_vga_seq_write(i686_VGA_SEQ_MAP_MASK, 0x04);
    i686_vga_seq_write(i686_VGA_SEQ_MEM_MODE, 0x06);

    // Switch GC to sequential (non-odd/even) read of plane 2
    i686_vga_gc_write(i686_VGA_GC_MODE, 0x00);
    i686_vga_gc_write(i686_VGA_GC_MISC, 0x04);  // map at 0xA0000, chain4/odd-even off

    for (uint16_t g = 0; g < glyph_count; g++) {
        for (uint8_t row = 0; row < font_height; row++) {
            font_plane[slot_offset + g * i686_VGA_FONT_GLYPH_SIZE + row]
                = data[g * font_height + row];
        }
    }

    // Restore text-mode sequencer and GC state
    i686_vga_seq_write(i686_VGA_SEQ_MAP_MASK, seq_regs_text[1]);
    i686_vga_seq_write(i686_VGA_SEQ_MEM_MODE, seq_regs_text[3]);
    i686_vga_gc_write(i686_VGA_GC_MODE, gc_regs_text[5]);
    i686_vga_gc_write(i686_VGA_GC_MISC, gc_regs_text[6]);
}
