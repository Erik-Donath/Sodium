#pragma once
#include <kernel/terminal.h>

extern const display_driver debug_driver;

bool i686_debug_check(void);
void i686_debug_activate(void);
void i686_debug_deactivate(void);
void i686_debug_putc(char);
void i686_debug_clear();
