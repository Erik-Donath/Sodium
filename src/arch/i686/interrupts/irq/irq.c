#include "kernel/libc/stdio.h"

#include "../../cpu/idt/idt.h"
#include "i8259A/pic.h"
#include "irq.h"

_Static_assert(i8259A_PIC2_OFFSET == i8259A_PIC1_OFFSET + 8, "PIC2 has to be behind PIC1");

#define defaultColor Color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK)
#define warnColor Color(TERMINAL_COLOR_LIGHT_BROWN, TERMINAL_COLOR_BLACK)

static IRQHandler handlers[16];

static void i686_IRQ_Handler(ISR_Registers* regs) {
    uint8_t irq = regs->interrupt - i8259A_PIC1_OFFSET;
    if(handlers[irq])
        handlers[irq](regs);
    else {
        printf("Unhandled IRQ %d\n", irq);
        printf(defaultColor "[ " warnColor "Warn" defaultColor " ] Device %d (int: 0x%X) is properly locked up\n", irq, regs->interrupt);
    }
    i8259A_SendEOI(irq);
}

i686_IRQ_ERROR_t i686_IRQ_Init() {
    if(!i8259A_Check()) {
        return i686_IRQ_ERROR_NO_DEVICE;
    }

    for(uint8_t i = 0; i < 16; i++) {
        i686_IDT_RegisterHandler(i8259A_PIC1_OFFSET + i, i686_IRQ_Handler);
    }

    i8259A_Enable();
    i686_IRQ_Enable();
    return i686_IRQ_ERROR_OK;
}

void i686_IRQ_RegisterHandler(uint8_t irq, IRQHandler handler) {
    if(irq < 16) {
        handlers[irq] = handler;
    }
}

void i686_IRQ_ClearHandler(uint8_t irq) {
    if(irq < 16) {
        handlers[irq] = 0;
    }
}
