#include <kernel/arch/i686/gdt.h>
#include <kernel/arch/i686/idt.h>
#include <kernel/arch/i686/isr.h>
#include <kernel/arch/i686/irq.h>
#include <kernel/arch/i686/e9.h>

#include "display.h"
#include "hal.h"

void HAL_Initilize() {
    GDT_Initilize();
    IDT_Initilize();
    ISR_Initilize();
    IRQ_Initilize();

    DISPLAY_Initilize();
}
