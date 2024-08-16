#include "io.h"

void iowait() {
    outb(IO_UNUSED_PORT, 0);
}
