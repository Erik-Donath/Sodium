#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct i686_isr_cpu_state {
    uint32_t gs, fs, es, ds;                                // segment registers
    uint32_t edi, esi, ebp, kernel_esp, ebx, edx, ecx, eax; // general-purpose
    uint32_t int_num;                                       // interrupt vector
    uint32_t error_code;                                    // CPU error code or 0
    uint32_t eip, cs, eflags, esp, ss;                      // CPU auto-push
} __attribute__((packed)) i686_isr_cpu_state_t;

typedef void (*i686_isr_handler_t)(i686_isr_cpu_state_t* state);
 
void i686_isr_init(void);

bool i686_isr_set_handler(uint8_t int_num, i686_isr_handler_t handler);
void i686_isr_clear_handler(uint8_t int_num);
