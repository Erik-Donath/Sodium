#pragma once

#include <stdint.h>

#define I686_I8259A_IRQ_SIZE    0x08
#define I686_I8259A_IRQ_MASTER  0x20
#define I686_I8259A_IRQ_SLAVE   (I686_I8259A_IRQ_MASTER + I686_I8259A_IRQ_SIZE)

// This Fill will change eventually when the new interrupt logic is in place

// @brief Initialise, remap, and unmask the i8259A dual-PIC.
//        Remaps IRQ 0–7 to vectors I686_I8259A_IRQ_MASTER..+7 and
//        IRQ 8–15 to I686_I8259A_IRQ_SLAVE..+7. Installs a default
//        handler on all 16 vectors and unmasks all pins.
void i686_i8259A_enable(void);

// @brief Mask all IRQ pins on both PICs.
void i686_i8259A_disable(void);

// @brief Mask (disable) a single IRQ pin.
// @param pin  0–15 (0–7 = master, 8–15 = slave).
void i686_i8259A_mask(uint8_t pin);

// @brief Unmask (enable) a single IRQ pin.
// @param pin  0–15.
void i686_i8259A_unmask(uint8_t pin);

// @brief Send specific EOI for the given IRQ pin.
//        Slave pins (8–15) also send EOI to the master cascade pin.
// @param pin  IRQ pin number relative to the master base.
void i686_i8259A_send_eoi(uint8_t pin);
