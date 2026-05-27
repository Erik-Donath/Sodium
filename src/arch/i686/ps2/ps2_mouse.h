#pragma once

#include "8042.h"

// Mouse driver instance.
// Register this with the 8042 driver table to handle devices with
// device ID I686_PS2_DEV_MOUSE (0x0000).
extern const i686_8042_driver_t i686_ps2_mouse_driver;
