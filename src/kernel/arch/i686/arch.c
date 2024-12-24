#include "arch.h"

#include "fpu.h"
#include "gdt.h"

void pre_main(void* mb_info) {
    i686_FPU_Initialize();
    i686_GDT_Initialize();
}
