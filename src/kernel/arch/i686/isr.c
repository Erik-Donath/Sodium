#include <stdint.h>
#include <stddef.h>
#include <kernel/stdio.h>
#include <kernel/hal/panic.h>

#include "idt.h"
#include "isr.h"

ISRHandler ISR_Handlers[256];

static const char* const ISR_Exceptions[] = {
    "Divide by zero error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception ",
    "",
    "",
    "",
    "",
    "",
    "",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    ""
};

void ISR_InitializeGates();
void ISR_Initilize() {
    ISR_InitializeGates();
    // ? Should this be deleted
    for(uint16_t i = 0; i < 256; i++)
        IDT_EnableGate((uint8_t)i);
}

void ISR_RegisterHandler(uint8_t interrupt, ISRHandler handler) {
    if(handler) {
        ISR_Handlers[interrupt] = handler;
        IDT_EnableGate(interrupt);
    }
    else {
        IDT_DisableGate(interrupt);
        ISR_Handlers[interrupt] = 0;
    }
}

void ASMCALL ISR_handler(ISR_Registers* regs) {
    ISRHandler handler = ISR_Handlers[regs->interrupt];
    if(handler)
        handler(regs);
    else if(regs->interrupt >= 32)
        printf("Unhandled interrupt %d:%d\n", regs->interrupt, regs->error);
    else {
        printf(
            "Unhandled exeption %s (%d:%d):\n",
             ISR_Exceptions[regs->interrupt], regs->interrupt, regs->error
        );
        printf(
            "  eax=%X  ebx=%X  ecx=%X  edx=%X  esi=%X  edi=%X\n",
            regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi
        );
        printf(
            "  esp=%X  ebp=%X  eip=%X  eflags=%X  cs=%X  ds=%X  ss=%X\n",
            regs->esp, regs->ebp, regs->eip, regs->eflags, regs->cs, regs->ds, regs->ss
        );
        printf(
            "  interrupt=%X  errorcode=%X\n",
            regs->interrupt, regs->error
        );

        // WE FUCKED UP!
        kernel_panic();
    }
}
