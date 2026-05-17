#include <kernel/hal.h>

#include "debug.h"

void hal_debug_putc(char c) {
    i686_debug_putc(c);
}
