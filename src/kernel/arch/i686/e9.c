#include "io.h"
#include "e9.h"

uint8_t E9_test() {
    return inb(E9_HACK);
}

void E9_putc(char c) {
    outb(E9_HACK, (uint8_t)c);
}