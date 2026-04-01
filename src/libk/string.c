#include <string.h>


size_t strlen(const char* str) {
    size_t len = 0;
    while(*str) {
        str++;
        len++;
    }
    return len;
}