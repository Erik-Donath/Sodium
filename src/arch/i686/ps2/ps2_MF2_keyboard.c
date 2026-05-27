#include "ps2_MF2_keyboard.h"

#include <stdio.h>

// TODO: Implement scan code parsing, key event dispatch,
//       and keyboard state tracking (shift, caps lock, etc.)

// Internal

// @brief Allocate driver context for this port.
// @return NULL until a context struct is defined.
static void *i686_ps2_mf2_kbd_open(uint8_t port);

// @brief Receive one raw MF2 scan code byte.
//        TODO: parse scan codes and dispatch key events.
// @param port  Source port.
// @param ctx   Context from open.
// @param byte  Raw scan code byte.
static void i686_ps2_mf2_kbd_data(uint8_t port, void *ctx, uint8_t byte);

// @brief Release driver context.
static void i686_ps2_mf2_kbd_close(uint8_t port, void *ctx);

// Definitions

static void *i686_ps2_mf2_kbd_open(uint8_t port) {
    printf("[INFO] Initilized MF2 Keyboard (0xAB83) Driver at port %u\n", port);
    return (void *)0;
}

static void i686_ps2_mf2_kbd_data(uint8_t port, void *ctx, uint8_t byte) {
    (void)port;
    (void)ctx;
    (void)byte;
}

static void i686_ps2_mf2_kbd_close(uint8_t port, void *ctx) {
    printf("[INFO] Removed MF2 Keyboard (0xAB83) Driver at port %u\n", port);
    (void)ctx;
}

const i686_8042_driver_t i686_ps2_mf2_kbd_driver = {
    .device_id = I686_PS2_DEV_MF2_KBD,
    .open      = i686_ps2_mf2_kbd_open,
    .data      = i686_ps2_mf2_kbd_data,
    .close     = i686_ps2_mf2_kbd_close,
};
