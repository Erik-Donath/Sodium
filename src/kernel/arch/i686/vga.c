#include "ports.h"
#include "vga.h"

#define VGA_BUFFER 0xB8000
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

// VGA I/O ports
#define VGA_MISC_OUTPUT_WRITE_PORT  0x3C2
#define VGA_MISC_OUTPUT_READ_PORT   0x3CC
#define VGA_SEQUENCER_ADDRESS_PORT  0x3C4
#define VGA_SEQUENCER_DATA_PORT     0x3C5
#define VGA_CRTC_ADDRESS_PORT       0x3D4
#define VGA_CRTC_DATA_PORT          0x3D5

// VGA controller registers
#define VGA_CURSOR_HIGH 0x0E
#define VGA_CURSOR_LOW  0x0F

static uint16_t  vga_cursor = 0;
static uint16_t* vga_buffer = (uint16_t*)VGA_BUFFER;

bool i686_vga_check() {
    outb(VGA_MISC_OUTPUT_WRITE_PORT, 0x63);
    outb(VGA_CRTC_ADDRESS_PORT, 0x03);
    outb(VGA_CRTC_DATA_PORT, inb(VGA_CRTC_DATA_PORT) | 0x80);

    uint16_t value = vga_buffer[0];
    vga_buffer[0] = 0x55AA;
    
    bool check = (vga_buffer[0] == 0x55AA);
    vga_buffer[0] = value;
    return check;
}

void i686_vga_init() {
    // Reset the controller
    outb(VGA_MISC_OUTPUT_WRITE_PORT, 0x67);

    // Miscellaneous output register
    outb(VGA_MISC_OUTPUT_WRITE_PORT, 0x63);

    // Sequencer registers
    outb(VGA_SEQUENCER_ADDRESS_PORT, 0x03);
    outb(VGA_SEQUENCER_DATA_PORT, 0x00);

    // Unlock CRTC registers
    outb(VGA_CRTC_ADDRESS_PORT, 0x03);
    outb(VGA_CRTC_DATA_PORT, inb(VGA_CRTC_DATA_PORT) | 0x80);
    outb(VGA_CRTC_ADDRESS_PORT, 0x11);
    outb(VGA_CRTC_DATA_PORT, inb(VGA_CRTC_DATA_PORT) & ~0x80);

    // Make sure they remain unlocked
    outb(VGA_CRTC_ADDRESS_PORT, 0x03);
    outb(VGA_CRTC_DATA_PORT, 0x03);

    // Set up CRTC registers for 80x25 text mode
    static const uint8_t crtc_regs[] = {
        0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
        0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50,
        0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
        0xFF
    };

    for (uint8_t i = 0; i < sizeof(crtc_regs); i++) {
        outb(VGA_CRTC_ADDRESS_PORT, i);
        outb(VGA_CRTC_DATA_PORT, crtc_regs[i]);
    }

    i686_vga_clear();
}

void i686_vga_putChar(char c, uint8_t color) {
    switch(c) {
        case '\n':
            i686_vga_newLine();
            break;
        case '\r':
            i686_vga_set_cursor_position((vga_cursor / VGA_WIDTH + 1) * VGA_WIDTH);
            break;
        case '\t':
            uint8_t tabs = 4 - (vga_cursor % VGA_WIDTH) % 4;
            for(uint8_t i = 0; i < tabs; i++) i686_vga_out(' ', color);
            break;
        default:
            i686_vga_out(c, color);
            break;
    }
}

void i686_vga_out(char c, uint8_t color) {
    vga_buffer[vga_cursor++] = (uint16_t)c | ((uint16_t)color << 8);
    if (vga_cursor >= VGA_WIDTH * VGA_HEIGHT) {
        i686_vga_scroll(1);
    }
}

void i686_vga_newLine() {
    vga_cursor = (vga_cursor / VGA_WIDTH + 1) * VGA_WIDTH;
    if(vga_cursor >= VGA_WIDTH * VGA_HEIGHT) {
        i686_vga_scroll(1);
    }
    i686_vga_set_cursor_position(vga_cursor);
}

void i686_vga_scroll(uint8_t lines) {
    if (lines == 0) return;
    if (lines > VGA_HEIGHT) lines = VGA_HEIGHT;

    // Move lines up
    for(uint8_t y = 0; y < VGA_HEIGHT - lines; y++) {
        for(uint8_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = vga_buffer[(y + lines) * VGA_WIDTH + x];
        }
    }

    // Clear bottom
    for(uint8_t y = VGA_HEIGHT - lines; y < VGA_HEIGHT; y++) {
        for(uint8_t x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = 0x0F20;
        }
    }

    // Adjust cursor
    if(vga_cursor > lines * VGA_WIDTH)
        vga_cursor -= lines * VGA_WIDTH;
    else vga_cursor = 0;
    i686_vga_set_cursor_position(vga_cursor);
}

void i686_vga_clear() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = 0x0F20;
    }
}

void i686_vga_set_cursor_position(uint16_t pos) {
    if(pos >= VGA_WIDTH * VGA_HEIGHT) {
        pos = VGA_WIDTH * VGA_HEIGHT - 1;
    }

    outb(VGA_CRTC_ADDRESS_PORT, VGA_CURSOR_HIGH);
    outb(VGA_CRTC_DATA_PORT, (pos >> 8) & 0xFF);
    outb(VGA_CRTC_ADDRESS_PORT, VGA_CURSOR_LOW);
    outb(VGA_CRTC_DATA_PORT, pos & 0xFF);
    vga_cursor = pos;
}

const display_driver vga_driver = {
    .name       = "Simple VGA Driver",
    .init       = i686_vga_init,
    .check      = i686_vga_check,
    .put_char   = i686_vga_putChar,
    .new_line   = i686_vga_newLine,
    .scroll     = i686_vga_scroll
};
