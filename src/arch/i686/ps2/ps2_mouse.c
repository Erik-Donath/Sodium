#include "ps2_mouse.h"

#include <stdio.h>

// TODO: Implement the driver

// Internal

// @brief Allocate driver context for this port.
// @return NULL until a context struct is defined.
static void *i686_ps2_mouse_open(uint8_t port);

// @brief Receive a code byte.
// @param port  Source port.
// @param ctx   Context from open.
// @param byte  Raw code byte.
static void i686_ps2_mouse_data(uint8_t port, void *ctx, uint8_t byte);

// @brief Release driver context.
static void i686_ps2_mouse_close(uint8_t port, void *ctx);

// Definitions

static void *i686_ps2_mouse_open(uint8_t port) {
    printf("[INFO] Initilized Stub Mouse (0x0000) Driver at port %u\n", port);
    return (void *)0;
}

static void i686_ps2_mouse_data(uint8_t port, void *ctx, uint8_t byte) {
    (void)port;
    (void)ctx;
    (void)byte;
}

static void i686_ps2_mouse_close(uint8_t port, void *ctx) {
    printf("[INFO] Removed Stub Mouse (0x0000) Driver at port %u\n", port);
    (void)ctx;
}

const i686_8042_driver_t i686_ps2_mouse_driver = {
    .device_id = I686_PS2_DEV_MOUSE,
    .open      = i686_ps2_mouse_open,
    .data      = i686_ps2_mouse_data,
    .close     = i686_ps2_mouse_close,
};