#include <stdarg.h>
#include <stdbool.h>

#include "hal/display.h"
#include "stdio.h"

static const char numberTable[] = "0123456789ABCDEF";

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
        print_unsigned(-value, radix);
    }
    else 
        print_unsigned((uint32_t)value, radix);
}

void print_unsigned(uint32_t value, uint8_t radix) {
    if(!radix || radix > 16) radix = 10;
    char buffer[32];
    int32_t pos = 0;

    do {
        buffer[pos++] = numberTable[value % radix];
        value = value / radix;
    } while(value > 0);

    // Print Buffer
    while(--pos >= 0) putc(buffer[pos]);
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
