#include "i8259A.h"

#include "../cpu/isr.h"
#include "../cpu/io.h"

#include <stdint.h>
#include <stdio.h>

// Internal

#define MASTER_CMD   0x0020
#define MASTER_DATA  0x0021
#define SLAVE_CMD    0x00A0
#define SLAVE_DATA   0x00A1

#define master_cmd(c)   i686_io_outb(MASTER_CMD,  (c))
#define master_data(c)  i686_io_outb(MASTER_DATA, (c))
#define slave_cmd(c)    i686_io_outb(SLAVE_CMD,   (c))
#define slave_data(c)   i686_io_outb(SLAVE_DATA,  (c))
#define pic_wait()      i686_io_wait()

#define MASTER_SLAVE_PIN  0x02

enum {
    ICW1_ICW4       = 0x01,
    ICW1_SINGLE     = 0x02,
    ICW1_INTERVAL4  = 0x04,
    ICW1_LEVEL      = 0x08,
    ICW1_INITIALIZE = 0x10,

    ICW3_MASTER_SLAVE_MASK = (1 << MASTER_SLAVE_PIN),
    ICW3_SLAVE_CASCADE_ID  = MASTER_SLAVE_PIN,

    ICW4_8086          = 0x01,
    ICW4_AUTO_EOI      = 0x02,
    ICW4_BUFFER_MASTER = 0x04,
    ICW4_BUFFER_SLAVE  = 0x00,
    ICW4_BUFFERED      = 0x08,
    ICW4_SFNM          = 0x10,

    CMD_INITIALIZE              = 0x10,
    CMD_END_OF_INTERRUPT        = 0x20,
    CMD_SPECIFIC_EOI            = 0x60,
    CMD_READ_IRR                = 0x0A,
    CMD_READ_ISR                = 0x0B,
};

// @brief Default handler for PIC vectors without a registered driver.
//        Logs the vector and IRQ number, then sends EOI.
static void i686_i8259A_default_handler(i686_isr_cpu_state_t *state);

// Definitions

static void i686_i8259A_default_handler(i686_isr_cpu_state_t *state) {
    uint8_t num = state->int_num;
    if (num >= I686_I8259A_IRQ_MASTER &&
        num <  (I686_I8259A_IRQ_SLAVE + I686_I8259A_IRQ_SIZE)) {
        uint8_t pin = num - I686_I8259A_IRQ_MASTER;
        printf("Unhandled PIC interrupt %d on pin %d\n", num, pin);
        i686_i8259A_send_eoi(pin);
    } else {
        printf("Warning: PIC default handler called for non-PIC vector %d\n",
               num);
    }
}

void i686_i8259A_enable(void) {
    master_cmd (ICW1_INITIALIZE | ICW1_ICW4); pic_wait();
    slave_cmd  (ICW1_INITIALIZE | ICW1_ICW4); pic_wait();
    master_data(I686_I8259A_IRQ_MASTER);       pic_wait();
    slave_data (I686_I8259A_IRQ_SLAVE);        pic_wait();
    master_data(ICW3_MASTER_SLAVE_MASK);       pic_wait();
    slave_data (ICW3_SLAVE_CASCADE_ID);        pic_wait();
    master_data(ICW4_8086);                    pic_wait();
    slave_data (ICW4_8086);                    pic_wait();

    for (uint8_t i = 0; i < (I686_I8259A_IRQ_SIZE * 2); i++)
        i686_isr_set_handler(I686_I8259A_IRQ_MASTER + i,
                             i686_i8259A_default_handler);

    master_data(0x00); pic_wait();
    slave_data (0x00); pic_wait();
}

void i686_i8259A_disable(void) {
    master_data(0xFF);
    slave_data (0xFF);
}

void i686_i8259A_mask(uint8_t pin) {
    if (pin >= I686_I8259A_IRQ_SIZE) {
        uint8_t p = pin - I686_I8259A_IRQ_SIZE;
        slave_data(i686_io_inb(SLAVE_DATA) | (uint8_t)(1 << p));
    } else {
        master_data(i686_io_inb(MASTER_DATA) | (uint8_t)(1 << pin));
    }
}

void i686_i8259A_unmask(uint8_t pin) {
    if (pin >= I686_I8259A_IRQ_SIZE) {
        uint8_t p = pin - I686_I8259A_IRQ_SIZE;
        slave_data(i686_io_inb(SLAVE_DATA) & (uint8_t)~(1 << p));
    } else {
        master_data(i686_io_inb(MASTER_DATA) & (uint8_t)~(1 << pin));
    }
}

void i686_i8259A_send_eoi(uint8_t pin) {
    if (pin >= I686_I8259A_IRQ_SIZE) {
        slave_cmd (CMD_SPECIFIC_EOI | (pin - I686_I8259A_IRQ_SIZE));
        master_cmd(CMD_SPECIFIC_EOI | MASTER_SLAVE_PIN);
    } else {
        master_cmd(CMD_SPECIFIC_EOI | pin);
    }
    pic_wait();
}
