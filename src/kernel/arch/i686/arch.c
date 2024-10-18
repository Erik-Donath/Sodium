#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"

#include <kernel/kernel.h>
#include "arch.h"

void pre_main(void* mb_info) {
    // Step 1: Initilize CPU, Interrupr Handling
    i686_GDT_Initilize();
    i686_IDT_Initilize();
    i686_ISR_Initilize();
    i686_IRQ_Initilize();

    // Step 2: Read System Information
    // ...

    // Step 3: Jump into generic Kernel
    kernel_main();
}
