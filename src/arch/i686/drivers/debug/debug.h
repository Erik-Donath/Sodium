#pragma once
#include <kernel/drivers/display.h>
#include <kernel/core/terminal.h>

extern const display_driver_t debug_driver;

bool i686_debug_check(void);
void i686_debug_activate(void);
void i686_debug_deactivate(void);
void i686_debug_putc(char);
void i686_debug_clear();
