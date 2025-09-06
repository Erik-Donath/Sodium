
#include "error.h"
#include <kernel/libc/stdio.h>
#include <kernel/libc/util.h>
#include "terminal.h"

// Architecture-specific shutdown function (implemented per architecture)
extern void NORETURN arch_shutdown(void);

// Print panic message and halt system
void NORETURN panic(const char* msg) {
    printf(DEFAULT_COLOR "[ " ERROR_COLOR "PANIC" DEFAULT_COLOR " ] %s\n", msg);
    arch_shutdown();
}

// Print detailed panic message with location and halt system
void NORETURN panic_detailed(const char* msg, const char* file, int line, const char* func) {
    printf(DEFAULT_COLOR "[ " ERROR_COLOR "PANIC" DEFAULT_COLOR " ] %s\n", msg);
    printf("Location: %s:%d in %s()\n", file, line, func);
    arch_shutdown();
}
