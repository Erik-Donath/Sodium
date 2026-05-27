#pragma once

#include <stdbool.h>
#include <stdint.h>

// CPU state pushed by the ISR stub and the CPU itself on interrupt entry.
typedef struct i686_isr_cpu_state {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, kernel_esp, ebx, edx, ecx, eax;
    uint32_t int_num;
    uint32_t error_code;
    uint32_t eip, cs, eflags, esp, ss;
} __attribute__((packed)) i686_isr_cpu_state_t;

typedef void (*i686_isr_handler_t)(i686_isr_cpu_state_t *state);

// @brief Install default handlers on all 256 vectors.
void i686_isr_init(void);

// @brief Register a handler for a vector.
//        Succeeds only if the slot holds the default or unhandled-exception stub.
// @param int_num  Vector (0–255).
// @param handler  Callback to install.
// @return true if installed; false if the slot was already occupied.
bool i686_isr_set_handler(uint8_t int_num, i686_isr_handler_t handler);

// @brief Restore the default stub for a vector.
// @param int_num  Vector to clear (0–255).
void i686_isr_clear_handler(uint8_t int_num);
