#include "util.h"
#include "terminal.h"
_Static_assert(sizeof(char) == sizeof(uint8_t), "char and uint8_t must be the same size!");

#include <kernel/arch/i686/vga/vga.h>
#include <kernel/arch/i686/debug/debug.h>

static const display_driver* terminal_drivers[] = {
    &vga_driver, &debug_driver
};

static display_driver* terminal_active_drivers[ARRAY_SIZE(terminal_drivers)];
static uint32_t terminal_active_driver_count;

void terminal_init() {
    terminal_active_driver_count = 0;
    for(uint32_t i = 0; i < ARRAY_SIZE(terminal_active_drivers); i++) {
        if(terminal_drivers[i]->check()) {
            display_driver* driver = terminal_active_drivers[terminal_active_driver_count++] = (display_driver*)terminal_drivers[i];
            driver->activate();
        }
    }
    for(uint32_t i = terminal_active_driver_count; i < ARRAY_SIZE(terminal_active_drivers); i++)
        terminal_active_drivers[i] = 0;
}


#define TS(x) #x
#define FOR_ALL_ACTIVE_DRIVERS(var, func) { \
    for(uint32_t i = 0; i < terminal_active_driver_count; i++) { \
        display_driver* var = terminal_active_drivers[i]; \
        func ;\
    } \
}

void terminal_putc(char c) {
    FOR_ALL_ACTIVE_DRIVERS(driver, driver->putc(c));
}

void terminal_puts(const char* str) {
    while(*str) {
        terminal_putc(*str);
        str++;
    }
}

void terminal_clear() {
    FOR_ALL_ACTIVE_DRIVERS(driver, driver->clear());
}