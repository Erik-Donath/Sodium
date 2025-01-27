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

#define setFG(color, fg) (((color) & 0xF0) | ((fg) & 0x0F))
#define setBG(color, bg) (((color) & 0x0F) | (((bg) & 0x0F) << 4))

#define CHAR(chr, color) (uint16_t)((color) << 8) | (uint16_t)(chr)
#define SCREEN_POS(x, y) (uint16_t)(y) * VGA_WIDTH + (uint16_t)(x)

typedef enum {
    READ        = 0,
    ESCAPE      = 1,
    PARAMETER   = 2,
} ASCII_Phase;

static ASCII_Phase vga_phase;

#define VGA_SEQUENZ_MAX 16
static uint8_t vga_sequenz_lenght;
static char vga_sequenz[VGA_SEQUENZ_MAX];

static const uint8_t color_map[256] = {
    0,  4,  2,  6,  1,  5,  3,  7,  8, 12, 10, 14,  9, 13, 11, 15,  // 16 Standard-Farben
    0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  // Blau-Töne
    0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  // Grün-Töne
    0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  // Cyan-Töne
    0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  // Rot-Töne
    0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  // Magenta-Töne
    0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  // Gelb-Töne
    0,  0,  0,  0,  7,  7,  7,  7,  8,  8,  8,  8, 15, 15, 15, 15,  // Grau-Töne
    0,  0,  0,  0,  0,  0,  0,  0,  8,  8,  8,  8,  8,  8,  8,  8,  // Hellere Grau-Töne
    8,  8,  8,  8,  8,  8,  8,  8,  7,  7,  7,  7,  7,  7,  7,  7,  // Noch hellere Grau-Töne
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15  // Weiß-Töne
};

bool i686_VGA_check() {
    outb(VGA_MISC_OUTPUT_WRITE_PORT, 0x63);
    outb(VGA_CRTC_ADDRESS_PORT, 0x03);
    outb(VGA_CRTC_DATA_PORT, inb(VGA_CRTC_DATA_PORT) | 0x80);

    uint16_t value = vga_buffer[0];
    vga_buffer[0] = 0x55AA;

    bool check = (vga_buffer[0] == 0x55AA);
    vga_buffer[0] = value;
    return check;
}

void i686_VGA_activate() {
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

    i686_VGA_clear();
    vga_phase = READ;
    vga_sequenz_lenght = 0;
}

void i686_VGA_deactivate() {
    // TODO: Implement later
}

void i686_VGA_putc(char c) {
    switch(vga_phase) {
        case READ:
            // Handle ESCAPE Characters. Otherwise print char normaly.
            switch(c) {
                case '\a':
                    break;
                case '\b':
                    i686_VGA_cursor_left();
                    break;
                case '\f':
                    i686_VGA_clear();
                    break;
                case '\n':
                    i686_VGA_newLine();
                    break;
                case '\r':
                    i686_VGA_cursor_set(0, vga_cursor_y);
                    break;
                case '\t':
                    uint8_t tabs = 4 - vga_cursor_x % 4;
                    for(uint8_t i = 0; i < tabs; i++)
                        i686_VGA_out(' ');
                    break;
                case '\v':
                    uint8_t skip = (4 - vga_cursor_x % 4) + VGA_WIDTH;
                    for(uint8_t i = 0; i < skip; i++)
                        i686_VGA_out(' ');
                    break;
                case '\033': // same as \e
                    vga_phase = ESCAPE;
                    break;
                default:
                    i686_VGA_out(c);
                    break;
                }
            break;
        case ESCAPE:
            switch(c) {
                case 'D':
                    i686_VGA_cursor_down();
                    vga_phase = READ;
                    break;
                case 'E':
                    i686_VGA_newLine();
                    vga_phase = READ;
                    break;
                case 'M':
                    i686_VGA_cursor_up();
                    vga_phase = READ;
                    break;
                case '[':
                    vga_sequenz_lenght = 0;
                    vga_phase = PARAMETER;
                    break;
                default:
                    i686_VGA_out('_');
                    i686_VGA_out('E');
                    vga_phase = READ;
                    break;
            }
            break;
        case PARAMETER:
            // End of Sequenz
            if (!(c >= '0' && c <= '9') && c != ';') {
                i686_VGA_parserSequnez(c);
                vga_phase = READ;
            }
            else if (vga_sequenz_lenght >= VGA_SEQUENZ_MAX) {
                i686_VGA_out('_');
                i686_VGA_out('E');
                vga_phase = READ;
            }
            else
                vga_sequenz[vga_sequenz_lenght++] = c;
            break;
    }
}

void i686_VGA_out(char c) {
    vga_buffer[SCREEN_POS(vga_cursor_x, vga_cursor_y)] = CHAR(c, vga_color);
    i686_VGA_cursor_right();
}

void i686_VGA_newLine() {
    uint8_t y = vga_cursor_y + 1;

    if(y >= VGA_HEIGHT) {
        i686_VGA_scroll(1);
        y = VGA_HEIGHT -1;
    }

    i686_VGA_cursor_set(0, y);
}

void i686_VGA_parserSequnez(char operation) {
    uint16_t params[VGA_SEQUENZ_MAX + 1]; // Worst case: ;;; n times.
    uint16_t param_value = 0;
    uint8_t  param_count = 0;
    
    for(uint8_t i = 0; i < vga_sequenz_lenght; i++) {
        if(vga_sequenz[i] == ';') {
            params[param_count++] = param_value;
            param_value = 0;
        }
        else param_value = param_value * 10 + (vga_sequenz[i] - '0');
    }
    params[param_count++] = param_value;

    switch(operation) {
        case 'm':
            i686_VGA_sequenz_setGraphicsMode(params, param_count);
            break;
        case 'H':
        case 'f':
            if(param_count >= 2)
                i686_VGA_cursor_set((uint8_t)params[0], (uint8_t)params[1]);
            break;
        case 'A':
            for(uint16_t i = 0; i < params[0]; i++)
                i686_VGA_cursor_up();
            break;
        case 'B':
            for(uint16_t i = 0; i < params[0]; i++)
                i686_VGA_cursor_down();
            break;
        case 'C':
            for(uint16_t i = 0; i < params[0]; i++)
                i686_VGA_cursor_right();
            break;
        case 'D':
            for(uint16_t i = 0; i < params[0]; i++)
                i686_VGA_cursor_left();
            break;
        case 'J':
            switch(params[0]) {
                case 0:
                    break;
                case 1:
                case 2:
                    i686_VGA_clear();
                    break;
                default:
                    i686_VGA_out('_');
                    i686_VGA_out('E');
                    break;
            }
            break;
        case 'K':
            switch(params[0]) {
                case 0:
                    break;
                case 1:
                case 2:
                    for(uint8_t x = 0; x < vga_cursor_x; x++)
                        vga_buffer[vga_cursor_y * VGA_WIDTH + x] = 0x0F20;
                    i686_VGA_cursor_set(0, vga_cursor_y);
                    break;
                default:
                    i686_VGA_out('_');
                    i686_VGA_out('E');
                    break;
            }
            break;
        default:
            i686_VGA_out('_');
            i686_VGA_out('E');
        break;
    }
}

void i686_VGA_sequenz_setGraphicsMode(uint16_t* params, uint16_t param_count) {
    for(uint16_t i = 0; i < param_count; i++) {
        /*
        IF: 38;5;n or 48;5;n at i+0, i+1, i+2
        */
        if((i+3) <= param_count && (params[i+0] == 38 || params[i+0] == 48) && params[i+1] == 5) {
            uint16_t type = params[i+0];
            uint16_t color = params[i+2];
            if(type == 38) vga_color = setFG(vga_color, color_map[color]);
            if(type == 48) vga_color = setBG(vga_color, color_map[color]);
            i += 2;
        }
        else {
            uint16_t n = params[i];
            if(n >= 30 && n <= 37) vga_color = setFG(vga_color, (uint8_t)(n - 30));
            else if(n >= 40 && n <= 47) vga_color = setBG(vga_color, (uint8_t)(n - 40));
            else if(n >= 90 && n <= 97) vga_color = setFG(vga_color, (uint8_t)(n - 82));
            else if(n >= 100 && n <= 107) vga_color = setBG(vga_color, (uint8_t)(n - 92));
            else {
                switch(n) {
                    case 0:
                        vga_color = 0x0F;
                        break;
                    case 7:
                        // TODO: Implement inverse color
                        break;
                    default:
                        i686_VGA_out('_');
                        i686_VGA_out('E');
                        break;
                }
            }
        }
    }
}

void i686_VGA_scroll(uint8_t lines) {
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

    i686_VGA_cursor_set(0, vga_cursor_y - lines);
}

void i686_VGA_clear() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = CHAR('\0', 0x0F);
    }
    i686_VGA_cursor_set(0, 0);
}

void i686_VGA_cursor_right() {
    uint8_t x = vga_cursor_x + 1;
    uint8_t y = vga_cursor_y;

    if(x >= VGA_WIDTH) {
        x = 0;
        y++;
    }
    if(y >= VGA_HEIGHT) {
        i686_VGA_scroll(1);
        y = VGA_HEIGHT -1;
    }

    i686_VGA_cursor_set(x, y);
}

void i686_VGA_cursor_left() {
    uint8_t x = vga_cursor_x - 1;
    uint8_t y = vga_cursor_y;

    if(x >= VGA_WIDTH) {
        x = VGA_WIDTH - 1;
        y--;
    }
    if(y >= VGA_HEIGHT) {
        y = 0;
    }

    i686_VGA_cursor_set(x, y);
}

void i686_VGA_cursor_up() {
    uint8_t x = vga_cursor_x;
    uint8_t y = vga_cursor_y;

    if(y != 0) y--;
    i686_VGA_cursor_set(x, y);
}

void i686_VGA_cursor_down() {
    uint8_t x = vga_cursor_x;
    uint8_t y = vga_cursor_y + 1;

    if(y >= VGA_HEIGHT) {
        i686_VGA_scroll(1);
        y = VGA_HEIGHT -1;
    }

    i686_VGA_cursor_set(x, y);
}

void i686_VGA_cursor_set(uint8_t x, uint8_t y) {
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

const display_driver vga_driver = {
    .name       = "ASCII VGA Driver",
    .check      = i686_VGA_check,
    .activate   = i686_VGA_activate,
    .deactivate = i686_VGA_deactivate,
    .putc       = i686_VGA_putc,
    .clear      = i686_VGA_clear
};
