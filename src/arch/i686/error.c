
#include <kernel/libc/util.h>
#include "arch.h"

// Architecture-specific shutdown routine
void NORETURN arch_shutdown(void) {
    i686_shutdown();
}
