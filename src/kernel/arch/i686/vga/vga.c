#include "../ports.h"
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

static uint8_t vga_cursor_x = 0, vga_cursor_y = 0;
static uint8_t vga_color = 0x0F;
static uint16_t* vga_buffer = (uint16_t*)VGA_BUFFER;

#define CHAR(chr, color) (uint16_t)((color) << 8) | (uint16_t)(chr)
#define SCREEN_POS(x, y) (uint16_t)(y) * VGA_WIDTH + (uint16_t)(x)

typedef enum {
    READ        = 0,
    ESCAPE      = 1,
    PARAMETER   = 2,
} ASCII_Phase;

static ASCII_Phase vga_phase;

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

void i686_vga_activate() {
    // TODO: Implement 'activate' part

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

    static const uint8_t crtc_regs[] = {
        [0] = 0x5F,   // Horizontal Total
        [1] = 0x4F,   // End Horizontal Display
        [2] = 0x50,   // Start Horizontal Blanking
        [3] = 0x82,   // End Horizontal Blanking
        [4] = 0x55,   // Start Horizontal Retrace
        [5] = 0x81,   // End Horizontal Retrace
        [6] = 0xBF,   // Vertical Total
        [7] = 0x1F,   // Overflow
        [8] = 0x00,   // Preset Row Scan
        [9] = 0x4F,   // Maximum Scan Line
        [10] = 0x0D,  // Cursor Start
        [11] = 0x0E,  // Cursor End
        [12] = 0x00,  // Start Address High
        [13] = 0x00,  // Start Address Low
        [14] = 0x00,  // Cursor Location High
        [15] = 0x50,  // Cursor Location Low
        [16] = 0x9C,  // Vertical Retrace Start
        [17] = 0x0E,  // Vertical Retrace End
        [18] = 0x8F,  // Vertical Display End
        [19] = 0x28,  // Offset
        [20] = 0x1F,  // Underline Location
        [21] = 0x96,  // Start Vertical Blanking
        [22] = 0xB9,  // End Vertical Blanking
        [23] = 0xA3,  // CRTC Mode Control
        [24] = 0xFF   // Line Compare
    }; // 80*25 text mode
    
    for (uint8_t i = 0; i < sizeof(crtc_regs); i++) {
        outb(VGA_CRTC_ADDRESS_PORT, i);
        outb(VGA_CRTC_DATA_PORT, crtc_regs[i]);
    }

    i686_vga_clear();
    vga_phase = READ;
}

void i686_vga_deactivate() {
    // TODO: Implement later
}

void i686_vga_putc(char c) {
    outb(0xE9, c);
    switch(vga_phase) {
        case READ:
            // Handle ESCAPE Characters. Otherwise print char normaly.
            // NOTE: \nnn and \xhh are not supported. (Oktal and Hex)
            switch(c) {
                case '\a':
                    break;
                case '\b':
                    i686_vga_cursor_backward();
                    break;
                case '\f':
                    i686_vga_clear();
                    break;
                case '\n':
                    i686_vga_set_cursor(0, vga_cursor_y + 1);
                    break;
                case '\r':
                    i686_vga_set_cursor(0, vga_cursor_y);
                    break;
                case '\t':
                    uint8_t tabs = 4 - vga_cursor_x % 4;
                    for(uint8_t i = 0; i < tabs; i++)
                        i686_vga_out(' ');
                    break;
                case '\v':
                    uint8_t skip = (4 - vga_cursor_x % 4) + VGA_WIDTH;
                    for(uint8_t i = 0; i < skip; i++)
                        i686_vga_out(' ');
                    break;
                case '\033': // same as \e
                    vga_phase = ESCAPE;
                    break;
                default:
                    i686_vga_out(c);
                    break;
                }
            break;
        case ESCAPE:
            vga_phase = READ;
            break;
        case PARAMETER:
            break;
    }
}

void i686_vga_out(char c) {
    vga_buffer[SCREEN_POS(vga_cursor_x, vga_cursor_y)] = CHAR(c, vga_color);
    i686_vga_cursor_forward();
}

void i686_vga_cursor_forward() {
    uint8_t x = vga_cursor_x + 1;
    uint8_t y = vga_cursor_y;

    if(x >= VGA_WIDTH) {
        x = 0;
        y++;
    }
    if(y >= VGA_HEIGHT) {
        i686_vga_scroll(1);
        y = VGA_HEIGHT -1;
    }

    i686_vga_set_cursor(x, y);
}

void i686_vga_cursor_backward() {
    uint8_t x = vga_cursor_x - 1;
    uint8_t y = vga_cursor_y;

    if(x >= VGA_WIDTH) {
        x = VGA_WIDTH - 1;
        y--;
    }
    if(y >= VGA_HEIGHT) {
        y = 0;
    }

    i686_vga_set_cursor(x, y);
}

void i686_vga_set_cursor(uint8_t x, uint8_t y) {
    if(x >= VGA_WIDTH)  x = VGA_WIDTH  - 1;
    if(y >= VGA_HEIGHT) y = VGA_HEIGHT - 1;

    uint16_t pos = SCREEN_POS(x, y);
    outb(VGA_CRTC_ADDRESS_PORT, VGA_CURSOR_HIGH);
    outb(VGA_CRTC_DATA_PORT, (pos >> 8) & 0xFF);
    outb(VGA_CRTC_ADDRESS_PORT, VGA_CURSOR_LOW);
    outb(VGA_CRTC_DATA_PORT, pos & 0xFF);

    vga_cursor_x = x;
    vga_cursor_y = y;
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

    vga_cursor_y -= lines;
}

void i686_vga_clear() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = CHAR('\0', 0x0F);
    }
    i686_vga_set_cursor(0, 0);
}

const display_driver vga_driver = {
    .name       = "ASCII VGA Driver",
    .check      = i686_vga_check,
    .activate   = i686_vga_activate,
    .deactivate = i686_vga_deactivate,
    .putc       = i686_vga_putc,
    .clear      = i686_vga_clear
};
