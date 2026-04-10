#include "i8259A.h"
#include "../cpu/isr.h"
#include "../io.h"
#include <stdint.h>
#include <stdio.h>

// IO Ports
#define MASTER_CMD_REG 0x0020
#define MASTER_DATA_REG 0x0021
#define SLAVE_CMD_REG 0x00A0
#define SLAVE_DATA_REG 0x00A1

#define master_command(cmd) i686_io_outb(MASTER_CMD_REG, (cmd))
#define master_data(cmd) i686_io_outb(MASTER_DATA_REG, (cmd))
#define slave_command(cmd) i686_io_outb(SLAVE_CMD_REG, (cmd))
#define slave_data(cmd) i686_io_outb(SLAVE_DATA_REG, (cmd))
#define wait() i686_io_wait()

enum ICW1 : uint8_t {
  ICW1_ICW4 = 0x01,
  ICW1_SINGLE = 0x02,
  ICW1_INTERVAL4 = 0x04,
  ICW1_LEVEL = 0x08,
  ICW1_INITIALIZE = 0x10,
};

#define MASTER_SLAVE_PIN 0x02

enum ICW3 : uint8_t {
  ICW3_MASTER_SLAVE_MASK = (1 << MASTER_SLAVE_PIN),
  ICW3_SLAVE_CASCADE_ID = MASTER_SLAVE_PIN,
};

enum ICW4 : uint8_t {
  ICW4_8086 = 0x01,
  ICW4_AUTO_EOI = 0x02,
  ICW4_BUFFER_MASTER = 0x04,
  ICW4_BUFFER_SLAVE = 0x00,
  ICW4_BUFFERRED = 0x08,
  ICW4_SFNM = 0x10,
};

enum CMD : uint8_t {
  CMD_INITIALIZE = ICW1_INITIALIZE,
  CMD_END_OF_INTERRUPT = 0x20,
  CMD_SPESIFIC_END_OF_INTERRUPT = 0x60,
  CMD_READ_IRR = 0x0A,
  CMD_READ_ISR = 0x0B,
};

static void i686_i8259A_isr_default_handler(i686_isr_cpu_state_t *state) {
  uint8_t num = state->int_num;
  if (num >= i686_i8259A_irq_master &&
      num < (i686_i8259A_irq_slave + i686_i8259A_irq_size)) {
    uint8_t port = num - i686_i8259A_irq_master;
    printf("Unhandled PIC Interrupt %d on PIC Port %d\n", num, port);
    i686_i8259A_send_eoi(port);
  } else {
    printf("Warning: Called PIC Default Handler for none PIC Interrupt. "
           "Called Interrupt %d ignored\n",
           num);
  }
}

void i686_i8259A_enable(void) {
  // 1. Send Initialize (CW1)
  master_command(ICW1_INITIALIZE | ICW1_ICW4);
  slave_command(ICW1_INITIALIZE | ICW1_ICW4);
  wait();

  // 2. Send Vektores (CW2)
  master_data(i686_i8259A_irq_master);
  slave_data(i686_i8259A_irq_slave);
  wait();

  // 3. Setup Cascade (CW3)
  master_data(ICW3_MASTER_SLAVE_MASK);
  slave_data(ICW3_SLAVE_CASCADE_ID);
  wait();

  // 4. Mode (CW4)
  master_data(ICW4_8086);
  slave_data(ICW4_8086);
  wait();

  // Set default handler
  for (uint8_t int_num = 0; int_num < (i686_i8259A_irq_size * 2); int_num++) {
    i686_isr_set_handler(i686_i8259A_irq_master + int_num,
                         i686_i8259A_isr_default_handler);
  }

  // Enable all interrupts
  master_data(0x00);
  slave_data(0x00);
  wait();
}

void i686_i8259A_disable(void) {
  master_data(0xFF);
  slave_data(0xFF);
}

void i686_i8259A_mask(uint8_t pin) {
  if (pin >= i686_i8259A_irq_size) {
    uint8_t slave_pin = pin - i686_i8259A_irq_size;
    slave_data(i686_io_inb(SLAVE_DATA_REG) | (1 << slave_pin));
  } else {
    master_data(i686_io_inb(MASTER_DATA_REG) | (1 << pin));
  }
}

void i686_i8259A_unmask(uint8_t pin) {
  if (pin >= i686_i8259A_irq_size) {
    uint8_t slave_pin = pin - i686_i8259A_irq_size;
    slave_data(i686_io_inb(SLAVE_DATA_REG) & ~(1 << slave_pin));
  } else {
    master_data(i686_io_inb(MASTER_DATA_REG) & ~(1 << pin));
  }
}

void i686_i8259A_send_eoi(uint8_t pin) {
  if (pin >= i686_i8259A_irq_size) {
    slave_command(CMD_SPESIFIC_END_OF_INTERRUPT | (pin - i686_i8259A_irq_size));
    master_command(CMD_SPESIFIC_END_OF_INTERRUPT | MASTER_SLAVE_PIN);
  } else {
    master_command(CMD_SPESIFIC_END_OF_INTERRUPT | pin);
  }
  wait();
}