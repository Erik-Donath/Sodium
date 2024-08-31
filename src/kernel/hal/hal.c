#include <kernel/arch/i686/gdt.h>
#include <kernel/arch/i686/idt.h>
#include <kernel/arch/i686/e9.h>

#include "display.h"
#include "hal.h"

void HAL_Initilize() {
    GDT_Initialize();
    IDT_Initilize();
    DISPLAY_init();
}
