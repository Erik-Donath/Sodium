#include <kernel/libc/util.h>
#include "arch.h"

void NORETURN arch_shutdown(void) {
    i686_shutdown();
}
