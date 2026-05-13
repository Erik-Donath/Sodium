#pragma once
#include <stdbool.h>
#include <stdint.h>

#define I686_8042_PORT_A     0
#define I686_8042_PORT_B     1
#define I686_8042_PORT_COUNT 2

typedef enum i686_ps2_device : uint16_t {
    I686_PS2_DEV_NONE         = 0x00FF,
    I686_PS2_DEV_AT_KBD       = 0x00FF,
    I686_PS2_DEV_MOUSE        = 0x0000,
    I686_PS2_DEV_MOUSE_SCROLL = 0x0003,
    I686_PS2_DEV_MOUSE_5BTN   = 0x0004,
    I686_PS2_DEV_MF2_KBD      = 0xAB83,
    I686_PS2_DEV_MF2_KBD_TR   = 0xAB41,
    I686_PS2_DEV_MF2_KBD_TR2  = 0xABC1,
} i686_ps2_device_t;

typedef struct i686_8042_driver {
    i686_ps2_device_t   device_id;
    void              *(*open )(uint8_t port);
    void               (*data )(uint8_t port, void *ctx, uint8_t byte);
    void               (*close)(uint8_t port, void *ctx);
} i686_8042_driver_t;

typedef enum i686_8042_result : uint8_t {
    i686_8042_OK           = 0,
    i686_8042_ERR_TIMEOUT  = 1,
    i686_8042_ERR_BAD_PORT = 2,
} i686_8042_result_t;

bool               i686_8042_init(void);
i686_8042_result_t i686_8042_write(uint8_t port, uint8_t data);
i686_8042_result_t i686_8042_read(uint8_t *out);
i686_8042_result_t i686_8042_close(uint8_t port);
i686_8042_result_t i686_8042_reset_port(uint8_t port);
