#pragma once

#include <stdbool.h>
#include <stdint.h>

#define I686_8042_PORT_A      0
#define I686_8042_PORT_B      1
#define I686_8042_PORT_COUNT  2

// PS/2 device identification codes returned by the identify sequence.
typedef enum i686_ps2_device : uint16_t {
    I686_PS2_DEV_NONE         = 0xFFFE,  // port present but no device responded
    I686_PS2_DEV_AT_KBD       = 0x00FF,  // legacy AT keyboard (no ID bytes); This could also be a mallfunctioning device.
    I686_PS2_DEV_MOUSE        = 0x0000,
    I686_PS2_DEV_MOUSE_SCROLL = 0x0003,
    I686_PS2_DEV_MOUSE_5BTN   = 0x0004,
    I686_PS2_DEV_MF2_KBD      = 0xAB83,
    I686_PS2_DEV_MF2_KBD_TR   = 0xAB41,
    I686_PS2_DEV_MF2_KBD_TR2  = 0xABC1,
} i686_ps2_device_t;

typedef struct i686_8042_driver {
    i686_ps2_device_t  device_id;
    void              *(*open )(uint8_t port);
    void               (*data )(uint8_t port, void *ctx, uint8_t byte);
    void               (*close)(uint8_t port, void *ctx);
} i686_8042_driver_t;

typedef enum i686_8042_result : uint8_t {
    i686_8042_OK           = 0,
    i686_8042_ERR_TIMEOUT  = 1,
    i686_8042_ERR_BAD_PORT = 2,
} i686_8042_result_t;

// @brief Initialise the i8042 controller: disable ports, flush, self-test,
//        probe and load drivers for any attached devices.
// @return true on success; false if the controller self-test fails.
bool i686_8042_init(void);

// @brief Send one byte to a PS/2 device.
// @param port  I686_8042_PORT_A or I686_8042_PORT_B.
// @param data  Byte to send.
i686_8042_result_t i686_8042_write(uint8_t port, uint8_t data);

// @brief Read one byte from the PS/2 output buffer.
// @param out  Receives the byte on success.
i686_8042_result_t i686_8042_read(uint8_t *out);

// @brief Disable IRQ, disable port, call driver close callback.
i686_8042_result_t i686_8042_close(uint8_t port);

// @brief Close and re-probe a port, loading a fresh driver if a device is present.
i686_8042_result_t i686_8042_reset_port(uint8_t port);
