#include "isr.h"

#include <stdio.h>

// Internal

_Static_assert(sizeof(i686_isr_cpu_state_t) == 76,
               "i686_isr_cpu_state_t must be 76 bytes.");

// Global table — referenced directly from isr.asm, must have external linkage.
i686_isr_handler_t i686_isr_handlers[256] = {0};

static const char *const isr_exception[32] = {
    [0]  = "Divide by zero",
    [1]  = "Debug",
    [2]  = "Non-maskable interrupt",
    [3]  = "Breakpoint",
    [4]  = "Overflow",
    [5]  = "Bound range exceeded",
    [6]  = "Invalid opcode",
    [7]  = "Device not available",
    [8]  = "Double fault",
    [9]  = "Coprocessor segment overrun",
    [10] = "Invalid TSS",
    [11] = "Segment not present",
    [12] = "Stack-segment fault",
    [13] = "General protection fault",
    [14] = "Page fault",
    [15] = "[Reserved]",
    [16] = "x87 floating-point exception",
    [17] = "Alignment check",
    [18] = "Machine check",
    [19] = "SIMD floating-point exception",
    [20] = "Virtualization exception",
    [21] = "Control protection exception",
    [22] = "[Reserved]", 
    [23] = "[Reserved]", 
    [24] = "[Reserved]",
    [25] = "[Reserved]", 
    [26] = "[Reserved]", 
    [27] = "[Reserved]",
    [28] = "Hypervisor injection exception",
    [29] = "VMM communication exception",
    [30] = "Security exception",
    [31] = "[Reserved]",
};

// @brief Fallback for IRQ vectors (32–255) without a registered driver.
static void i686_isr_default_handler(i686_isr_cpu_state_t *state);

// @brief Fallback for CPU exceptions (0–31) without a registered handler.
//        TODO: replace with k_panic() once that subsystem exists.
static void i686_isr_unhandled_exception(i686_isr_cpu_state_t *state);

// Definitions

static void i686_isr_default_handler(i686_isr_cpu_state_t *state) {
    printf("Unhandled interrupt %d\n", state->int_num);
}

static void i686_isr_unhandled_exception(i686_isr_cpu_state_t *state) {
    printf("Unhandled exception %s (%d:%d):\n",
           isr_exception[state->int_num], state->int_num, state->error_code);
    printf("  gs=%X  fs=%X  es=%X  ds=%X\n",
           state->gs, state->fs, state->es, state->ds);
    printf("  edi=%X  esi=%X  ebp=%X  ebx=%X  edx=%X  ecx=%X  eax=%X\n",
           state->edi, state->esi, state->ebp,
           state->ebx, state->edx, state->ecx, state->eax);
    printf("  eip=%X  cs=%X  eflags=%X  esp=%X  ss=%X\n",
           state->eip, state->cs, state->eflags, state->esp, state->ss);
    // TODO: Call k_panic()
}

void i686_isr_init(void) {
    for (int i = 0; i < 256; i++)
        i686_isr_clear_handler(i);
}

bool i686_isr_set_handler(uint8_t int_num, i686_isr_handler_t handler) {
    bool free = i686_isr_handlers[int_num] == i686_isr_default_handler
             || i686_isr_handlers[int_num] == i686_isr_unhandled_exception;
    if (free)
        i686_isr_handlers[int_num] = handler;
    return free;
}

void i686_isr_clear_handler(uint8_t int_num) {
    i686_isr_handlers[int_num] = (int_num < 32)
        ? i686_isr_unhandled_exception
        : i686_isr_default_handler;
}
