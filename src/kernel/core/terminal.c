#include <kernel/libc/util.h>
#include <kernel/memory/memory.h>
#include "terminal.h"

_Static_assert(sizeof(char) == sizeof(uint8_t), "char and uint8_t must be the same size!");

static display_driver_t** drivers;
static bool* active_drivers;
static size_t driver_count;


void terminal_init(const display_driver_t** display_drivers, size_t arraySize) {
    display_drivers = display_drivers;
    active_drivers = malloc(arraySize * sizeof(bool));
    driver_count = arraySize;
    for(size_t i = 0; i < arraySize; i++) {
        active_drivers[i] = display_drivers[i]->check();
        if(active_drivers[i]) {
            display_drivers[i]->activate();
            display_drivers[i]->clear();
        }
    }
}


#define TS(x) #x
#define FOR_ALL_ACTIVE_DRIVERS(var, func) { \
    for(uint32_t i = 0; i < driver_count; i++) { \
        if(active_drivers[i]) { \
        display_driver_t* var = drivers[i]; \
        func ;\
    }} \
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

void terminal_testColor() {
    const char* table[16] = {
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"
    };

    terminal_puts("\033[0;48;5;0m");
    for(uint8_t fg = 0; fg <= 15; fg++) {
        terminal_puts("\033[38;5;");
        terminal_puts(table[fg]);
    	terminal_puts("m#");

        if((fg+1) % 8 == 0) terminal_puts("\033[0m\n\033[48;5;0m");
    }
}
