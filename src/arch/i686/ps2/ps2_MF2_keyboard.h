#pragma once

#include "8042.h"

// MF2 keyboard driver instance.
// Register this with the 8042 driver table to handle devices with
// device ID I686_PS2_DEV_MF2_KBD (0xAB83).
extern const i686_8042_driver_t i686_ps2_mf2_kbd_driver;
