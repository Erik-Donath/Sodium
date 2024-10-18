#include <kernel/kernel.h>
#include "arch.h"


void pre_main(void* mb_info) {
    kernel_main();
}