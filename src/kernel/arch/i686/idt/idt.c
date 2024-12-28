#include <stdint.h>

#include <kernel/stdio.h>
#include <kernel/util.h>

#include "../gdt/gdt.h"
#include "idt.h"

/*
| Bits    | Description                             |
|---------|-----------------------------------------|
| 0-15    | Offset (lower 16 bits)                  |
| 16-31   | Selector (segment selector)             |
| 32-39   | Reserved (must be zero)                 |
| 40-43   | Type (gate type: task, interrupt, trap) |
| 44      | Storage Segment (always 0 for IDT)      |
| 45-46   | Descriptor Privilege Level (DPL)        |
| 47      | Present flag (P)                        |
| 48-63   | Offset (higher 16 bits)                 |
*/

typedef struct {
    uint16_t base_low;
    uint16_t segment_selector;
    uint8_t  reserved;
    uint8_t  flags;
    uint16_t base_high;
} PACKED idt_entry;

typedef struct {
    uint16_t limit;
    idt_entry* base;
} PACKED idt_pointer;

#define IDT_ENTRY(isr, selector, flags) { \
    (uint16_t)((uint32_t)(isr) & 0xFFFF), \
    (selector), \
    (0), \
    (flags), \
    (uint16_t)(((uint32_t)(isr) >> 16) & 0xFFFF) \
}

static const char* const ISR_Exceptions[] = {
    [0] = "Divide by zero error",
    [1] = "Debug",
    [2] = "Non-maskable Interrupt",
    [3] = "Breakpoint",
    [4] = "Overflow",
    [5] = "Bound Range Exceeded",
    [6] = "Invalid Opcode",
    [7] = "Device Not Available (No Math Coprocessor)",
    [8] = "Double Fault",
    [9] = "Coprocessor Segment Overrun", // Reserved, not used
    [10] = "Invalid TSS",
    [11] = "Segment Not Present",
    [12] = "Stack-Segment Fault",
    [13] = "General Protection Fault",
    [14] = "Page Fault",
    [15] = "[Reserved]",
    [16] = "x87 Floating-Point Exception",
    [17] = "Alignment Check",
    [18] = "Machine Check",
    [19] = "SIMD Floating-Point Exception",
    [20] = "Virtualization Exception",
    [21] = "Control Protection Exception",
    [22] = "[Reserved]",
    [23] = "[Reserved]",
    [24] = "[Reserved]",
    [25] = "[Reserved]",
    [26] = "[Reserved]",
    [27] = "[Reserved]",
    [28] = "Hypervisor Injection Exception",
    [29] = "VMM Communication Exception",
    [30] = "Security Exception",
    [31] = "[Reserved]"
};

static idt_entry idt[256];
static idt_pointer idt_ptr = {
    .limit = sizeof(idt) - 1,
    .base = idt
};

void i686_IDT_SetGate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num] = (idt_entry){
        .base_low = base & 0xFFFF,
        .segment_selector = selector,
        .reserved = 0,
        .flags = flags,
        .base_high = (base >> 16) & 0xFFFF
    };
}

void C_CALL i686_IDT_Flush(idt_pointer* ptr);
void i686_ISR_Initialize();

void i686_IDT_Initialize() {
    i686_ISR_Initialize();
    i686_IDT_Flush(&idt_ptr);
}

void ASM_CALL i686_IDT_Handler(ISR_Registers* regs) {
    if(regs->interrupt >= 32)
        printf("Unhandled interrupt %d:%d\n", regs->interrupt, regs->error);
    else {
        printf(
            "Unhandled exeption %s (%d:%d):\n",
             ISR_Exceptions[regs->interrupt], regs->interrupt, regs->error
        );

        printf(
            "  gs=%X  fs=%X  es=%X  ds=%X\n",
            regs->gs, regs->fs, regs->es, regs->ds
        );
        printf(
            "  edi=%X  esi=%X  ebp=%X  ebx=%X  edx=%X  ecx=%X  eax=%X\n",
            regs->edi, regs->esi, regs->ebp, regs->ebx, regs->edx, regs->ecx, regs->eax
        );
        printf(
            "  eip=%X  cs=%X  eflags=%X  esp=%X  ss=%X\n",
            regs->eip, regs->cs, regs->eflags, regs->esp, regs->ss
        );

        // We fucked up!
    }
}
