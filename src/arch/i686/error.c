#include "kernel/core/error.h"
#include "kernel/libc/util.h"
#include "kernel/libc/stdio.h"
#include "arch.h"

#define defaultColor Color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK)
#define errorColor Color(TERMINAL_COLOR_RED, TERMINAL_COLOR_BLACK)

void NORETURN panic(const char* msg) {
    printf("%s[ %sPANIC%s ] %s\n", defaultColor, errorColor, defaultColor, msg);

    i686_shutdown();
}

void NORETURN panic_detailed(const char* msg, const char* file, int line, const char* func) {
    printf("%s[ %sPANIC%s ] %s\n", defaultColor, errorColor, defaultColor, msg);
    printf("Location: %s:%d in %s()\n", file, line, func);

    i686_shutdown();
}
