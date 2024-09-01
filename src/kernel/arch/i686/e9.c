#include "io.h"
#include "e9.h"

uint8_t E9_read() {
    return inb(E9_HACK);
}

void E9_write(char c) {
    outb(E9_HACK, (uint8_t)c);
}