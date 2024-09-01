#include "io.h"
#include "vga.h"

#define SCREEN_BUFFER ((uint16_t*)VGA_SCREEN_BUFFER_ADDRESS)

void VGA_Initilize() {
    // TODO: VGA driver has yet to be implemented!
}

uint8_t VGA_getScreenHeight() {
    return VGA_SCREEN_HEIGHT;
}

uint8_t VGA_getScreenWidth() {
    return VGA_SCREEN_WIDTH;
}

void VGA_setChar(uint16_t pos, uint16_t chr) {
    SCREEN_BUFFER[pos] = chr;
}

uint16_t VGA_getChar(uint16_t pos) {
    return SCREEN_BUFFER[pos];
}

void VGA_setCursor(uint16_t pos) {
    outb(VGA_CRTC_INDEX_PORT, 0x0F);
    outb(VGA_CRTC_DATA_PORT, (uint8_t)(pos & 0xFF));
    outb(VGA_CRTC_INDEX_PORT, 0x0E);
    outb(VGA_CRTC_DATA_PORT, (uint8_t)((pos >> 8) & 0xFF));
}
