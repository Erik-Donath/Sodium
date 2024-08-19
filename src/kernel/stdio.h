#pragma once

/*
    Printf Specifier:
        %% -> %
        %s -> String
        %c -> Char
        %d, %i -> Signed value
        %u -> Unsigned value
        %o -> Octal value
        %X -> Hex value
        %p -> Pointer
        %n -> Nothing

    value can be up to 32 bit.
*/

void putc(char c);
void puts(const char* str);
void print_signed(int32_t value, uint8_t radix);
void print_unsigned(uint32_t value, uint8_t radix);
void printf(const char* fmt, ...);
