#include "io.h"
#include "e9.h"

void i686_E9_Write(char c) {
    outb(E9_HACK, (uint8_t)c);
}