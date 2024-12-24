/*
#include <stdarg.h>
#include <stdbool.h>

#include "hal/display.h"
#include "util/arrays.h"
#include "stdio.h"

static const char numberTable[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void putc(char c) {
    DISPLAY_putc(c);
}

void puts(const char* str) {
    while(*str) {
        putc(*str);
        str++;
    }
}

void print_signed(int32_t value, uint8_t radix) {
    if(value < 0) {
        putc('-');
        print_unsigned((uint32_t)(-value), radix);
    }
    else 
        print_unsigned((uint32_t)value, radix);
}

void print_unsigned(uint32_t value, uint8_t radix) {
    if(radix < 2 || radix > ARRAY_SIZE(numberTable)) radix = 10;
    char buffer[32];
    int32_t pos = 0;

    do {
        buffer[pos++] = numberTable[value % radix];
        value = value / radix;
    } while(value > 0);

    // Print Buffer
    while(--pos >= 0) putc(buffer[pos]);
}

void print_hex32(uint32_t value, bool fill) {
    bool firstDigit = false;
    for(uint8_t i = 0; i < 8; i++) {
        uint8_t digit = (uint8_t)(value >> 28);
        if(digit || fill)
            firstDigit = true;
        if(firstDigit)
            putc(numberTable[digit]);
        value = value << 4;
    }
    if(!firstDigit)
        putc(numberTable[0]);
}

void print_hex64(uint64_t value, bool fill) {
    uint32_t v1 = value & 0x00000000FFFFFFFF;
    uint32_t v2 = value >> 32;

    if(v2) {
        print_hex32(v2, fill);
        print_hex32(v1, true);
    }
    else {
        print_hex32(v1, fill);
    }
}

void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    bool inOp = false;
    
    while(*fmt) {
        if(inOp) {
            switch(*fmt) {
                case '%': // %% -> '%'
                    putc('%');
                    break;
                case 's': // %s -> String
                    puts(va_arg(args, const char*));
                    break;
                case 'c': // %c -> Char
                    putc((char)va_arg(args, int));
                    break;
                case 'd': // %d -> Signed value
                case 'i': // %i -> Signed value
                    print_signed(va_arg(args, int32_t), 10);
                    break;
                case 'u': // %u -> Unsigned value
                    print_unsigned(va_arg(args, uint32_t), 10);
                    break;
                case 'o': // %o -> Octal
                    print_unsigned(va_arg(args, uint32_t), 8);
                    break;
                case 'X': // %X -> Hex
                    print_unsigned(va_arg(args, uint32_t), 16);
                    break;
                case 'B':
                    print_unsigned(va_arg(args, uint32_t), 2);
                    break;
                case 'p': // %p -> Pointer
                    puts("0x");
                    print_unsigned((uint32_t)va_arg(args, void*), 16);
                    break;
                case 'n':  // %n -> Nothing
                    break;
                default:
                    printf("[Not Implemented: %%%c]", *fmt);
                    break;
            }
            inOp = false;
        } 
        else if (*fmt == '%')
            inOp = true;
        else
            putc(*fmt); 
        fmt++;
    }

    if(inOp)
        putc('%');
    va_end(args);
}

void printf_test() {
    printf("prinf Test:\n");
    printf("%%%% -> %%\n");
    printf("%%s -> %s\n", "String");
    printf("%%c -> %c\n", 'C');
    printf("%%d, %%i -> %d, %i\n", 42, -36);
    printf("%%u -> %u\n", 1337);
    printf("%%o -> %o\n", 34);
    printf("%%X -> %X\n", 4919);
    printf("%%B -> %B\n", 42);
    printf("%%p -> %p\n", 0x4242);
    printf("%%n -> %n\n");
}
*/