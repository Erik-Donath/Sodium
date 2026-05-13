#include "ps2_MF2_keyboard.h"

// #FIXME: Stub driver. Implement scan code parsing, key event dispatch,
// and keyboard state tracking (shift, caps lock, etc.) here.

static void *i686_ps2_kbd_open(uint8_t port) {
    (void)port;
    return (void *)0;
}

static void i686_ps2_kbd_data(uint8_t port, void *ctx, uint8_t byte) {
    (void)port;
    (void)ctx;
    (void)byte;
}

static void i686_ps2_kbd_close(uint8_t port, void *ctx) {
    (void)port;
    (void)ctx;
}

const i686_8042_driver_t i686_ps2_kbd_driver = {
    .device_id = I686_PS2_DEV_MF2_KBD,
    .open      = i686_ps2_kbd_open,
    .data      = i686_ps2_kbd_data,
    .close     = i686_ps2_kbd_close,
};
