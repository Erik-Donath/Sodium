#pragma once

typedef struct i686_isr_cpu_state {
} __attribute__((packed)) i686_isr_cpu_state_t;

void i686_isr_init(void);