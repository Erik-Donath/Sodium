#pragma once

#include "../cpu/io.h"

#define I686_VGA_TEXT_BASE   0xB8000
#define I686_VGA_GRAPH_BASE  0xA0000

#define I686_VGA_PORT_MISC_WRITE  0x3C2
#define I686_VGA_PORT_MISC_READ   0x3CC
#define I686_VGA_PORT_SEQ_INDEX   0x3C4
#define I686_VGA_PORT_SEQ_DATA    0x3C5
#define I686_VGA_PORT_GC_INDEX    0x3CE
#define I686_VGA_PORT_GC_DATA     0x3CF
#define I686_VGA_PORT_CRTC_INDEX  0x3D4
#define I686_VGA_PORT_CRTC_DATA   0x3D5
#define I686_VGA_PORT_AC_INDEX    0x3C0   // write index AND data (AC flip-flop)
#define I686_VGA_PORT_AC_READ     0x3C1
#define I686_VGA_PORT_INSTAT1     0x3DA   // read resets AC flip-flop

enum {
    I686_VGA_MISC_IO_COLOR    = 0x01,  // CRTC at 0x3D4 (colour mode)
    I686_VGA_MISC_RAM_ENABLE  = 0x02,  // CPU access to video RAM
    I686_VGA_MISC_CLK_25MHZ   = 0x00,  // 25.175 MHz pixel clock (640px)
    I686_VGA_MISC_CLK_28MHZ   = 0x04,  // 28.322 MHz pixel clock (720px)
    I686_VGA_MISC_ODD_PAGE    = 0x20,
    I686_VGA_MISC_HSYNC_NEG   = 0x40,
    I686_VGA_MISC_VSYNC_NEG   = 0x80,
};

enum {
    I686_VGA_SEQ_RESET    = 0x00,
    I686_VGA_SEQ_CLOCKING = 0x01,
    I686_VGA_SEQ_MAP_MASK = 0x02,
    I686_VGA_SEQ_CHAR_MAP = 0x03,
    I686_VGA_SEQ_MEM_MODE = 0x04,

    I686_VGA_SEQ_RESET_SYNC  = 0x01,
    I686_VGA_SEQ_RESET_CLEAR = 0x03,
};

enum {
    I686_VGA_CRTC_H_TOTAL       = 0x00,
    I686_VGA_CRTC_H_DISP_END    = 0x01,
    I686_VGA_CRTC_H_BLANK_START = 0x02,
    I686_VGA_CRTC_H_BLANK_END   = 0x03,
    I686_VGA_CRTC_H_SYNC_START  = 0x04,
    I686_VGA_CRTC_H_SYNC_END    = 0x05,
    I686_VGA_CRTC_V_TOTAL       = 0x06,
    I686_VGA_CRTC_OVERFLOW      = 0x07,
    I686_VGA_CRTC_PRESET_ROW    = 0x08,
    I686_VGA_CRTC_MAX_SCAN_LINE = 0x09,
    I686_VGA_CRTC_CURSOR_START  = 0x0A,
    I686_VGA_CRTC_CURSOR_END    = 0x0B,
    I686_VGA_CRTC_START_HI      = 0x0C,
    I686_VGA_CRTC_START_LO      = 0x0D,
    I686_VGA_CRTC_CURSOR_HI     = 0x0E,
    I686_VGA_CRTC_CURSOR_LO     = 0x0F,
    I686_VGA_CRTC_V_SYNC_START  = 0x10,
    I686_VGA_CRTC_V_SYNC_END    = 0x11,
    I686_VGA_CRTC_V_DISP_END    = 0x12,
    I686_VGA_CRTC_OFFSET        = 0x13,
    I686_VGA_CRTC_UNDERLINE     = 0x14,
    I686_VGA_CRTC_V_BLANK_START = 0x15,
    I686_VGA_CRTC_V_BLANK_END   = 0x16,
    I686_VGA_CRTC_MODE_CTRL     = 0x17,
    I686_VGA_CRTC_LINE_COMPARE  = 0x18,

    I686_VGA_CRTC_PROTECT_BIT   = 0x80,
    I686_VGA_CURSOR_DISABLE_BIT = 0x20,
};

enum {
    I686_VGA_GC_SR_VALUE        = 0x00,
    I686_VGA_GC_SR_ENABLE       = 0x01,
    I686_VGA_GC_COLOR_COMPARE   = 0x02,
    I686_VGA_GC_DATA_ROTATE     = 0x03,
    I686_VGA_GC_READ_MAP        = 0x04,
    I686_VGA_GC_MODE            = 0x05,
    I686_VGA_GC_MISC            = 0x06,
    I686_VGA_GC_COLOR_DONT_CARE = 0x07,
    I686_VGA_GC_BIT_MASK        = 0x08,
};

enum {
    I686_VGA_AC_PALETTE_BASE    = 0x00,
    I686_VGA_AC_MODE_CTRL       = 0x10,
    I686_VGA_AC_OVERSCAN        = 0x11,
    I686_VGA_AC_COLOR_PLANE_EN  = 0x12,
    I686_VGA_AC_H_PAN           = 0x13,
    I686_VGA_AC_COLOR_SELECT    = 0x14,

    I686_VGA_AC_MODE_BLINK_ENABLE = 0x08,
    I686_VGA_AC_MODE_9DOT_CHARS   = 0x04,
    I686_VGA_AC_PALETTE_ENABLE    = 0x20,
};

#define I686_VGA_FONT_SLOT_0_OFFSET  0x0000
#define I686_VGA_FONT_SLOT_1_OFFSET  0x4000
#define I686_VGA_FONT_GLYPH_SIZE     32

// @brief Write a Sequencer register.
static inline void i686_vga_seq_write(uint8_t index, uint8_t value) {
    i686_io_outb(I686_VGA_PORT_SEQ_INDEX, index);
    i686_io_outb(I686_VGA_PORT_SEQ_DATA,  value);
}

// @brief Write a CRTC register.
static inline void i686_vga_crtc_write(uint8_t index, uint8_t value) {
    i686_io_outb(I686_VGA_PORT_CRTC_INDEX, index);
    i686_io_outb(I686_VGA_PORT_CRTC_DATA,  value);
}

// @brief Read a CRTC register.
static inline uint8_t i686_vga_crtc_read(uint8_t index) {
    i686_io_outb(I686_VGA_PORT_CRTC_INDEX, index);
    return i686_io_inb(I686_VGA_PORT_CRTC_DATA);
}

// @brief Write a Graphics Controller register.
static inline void i686_vga_gc_write(uint8_t index, uint8_t value) {
    i686_io_outb(I686_VGA_PORT_GC_INDEX, index);
    i686_io_outb(I686_VGA_PORT_GC_DATA,  value);
}

// @brief Reset the AC flip-flop to index state by reading INSTAT1.
//        Must be called before i686_vga_ac_write() if the AC state is unknown.
static inline void i686_vga_ac_reset_flipflop(void) {
    i686_io_inb(I686_VGA_PORT_INSTAT1);
}

// @brief Write an Attribute Controller register.
//        Assumes the AC flip-flop is in index state.
static inline void i686_vga_ac_write(uint8_t index, uint8_t value) {
    i686_io_outb(I686_VGA_PORT_AC_INDEX, index | I686_VGA_AC_PALETTE_ENABLE);
    i686_io_outb(I686_VGA_PORT_AC_INDEX, value);
}
