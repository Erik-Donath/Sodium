#include "io.h"
#include "pic.h"
#include <kernel/stdio.h>

#include "irq.h"

#define PIC_REMAP_OFFSET    0x20

static IRQHandler IRQ_Handlers[16];

static void IRQ_Handler(ISR_Registers* regs) {
    uint8_t irq = regs->interrupt - PIC_REMAP_OFFSET;
    IRQHandler handler = IRQ_Handlers[irq];
    if(handler)
        handler(regs);
    else
        printf("Unhandled IRQ %d\n", irq);
    
    PIC_SendEOI(irq);
}

void i686_IRQ_Initilize() {
    PIC_Configure(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8);

    //register isr handler
    for(uint8_t i = 0; i < 16; i++) {
        i686_ISR_RegisterHandler(PIC_REMAP_OFFSET + i, IRQ_Handler);
    }

    // enable interrupts
    EnableInterrupts();

}
void i686_IRQ_RegisterHandler(uint8_t irq, IRQHandler handler) {
    IRQ_Handlers[irq] = handler;
}