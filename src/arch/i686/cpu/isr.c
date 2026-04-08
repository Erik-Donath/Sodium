#include "isr.h"

#include <stdio.h>

i686_isr_handler_t i686_isr_handlers[256] = { 0 };

_Static_assert(sizeof(i686_isr_cpu_state_t) == 76, "i686_isr_cpu_state_t must be 72 bytes long");

static const char* const isr_exception[32] = {
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
    [31] = "[Reserved]",
};

static void i686_isr_default_handler(i686_isr_cpu_state_t* state) {
    printf("Unhandled interrupt %d\n", state->int_num);
}
 
static void i686_isr_unhandled_exception(i686_isr_cpu_state_t* state) {
    printf(
        "Unhandled exception %s (%d:%d):\n",
        isr_exception[state->int_num], state->int_num, state->error_code
    );

    printf(
        "  gs=%X  fs=%X  es=%X  ds=%X\n",
        state->gs, state->fs, state->es, state->ds
    );
    printf(
        "  edi=%X  esi=%X  ebp=%X  ebx=%X  edx=%X  ecx=%X  eax=%X\n",
        state->edi, state->esi, state->ebp, state->ebx, state->edx, state->ecx, state->eax
    );
    printf(
        "  eip=%X  cs=%X  eflags=%X  esp=%X  ss=%X\n",
        state->eip, state->cs, state->eflags, state->esp, state->ss
    );

    // #FIXME: Call panic
}

void i686_isr_init(void) {
    for(int i = 0; i < 256; i++)
        i686_isr_clear_handler(i);
}

void i686_isr_set_handler(uint8_t int_num, i686_isr_handler_t handler) {
    if(i686_isr_handlers[int_num] == i686_isr_default_handler || i686_isr_handlers[int_num] == i686_isr_unhandled_exception) {
        i686_isr_handlers[int_num] = handler;
    }
    else {
        printf("Tried Overwritting existing Interrupt %d (CANCELD)\n", int_num);
        // #FIXME: Add kernel logging.
    }
}

void i686_isr_clear_handler(uint8_t int_num) {
    i686_isr_handlers[int_num] = (int_num < 32) ? i686_isr_unhandled_exception : i686_isr_default_handler;
}
