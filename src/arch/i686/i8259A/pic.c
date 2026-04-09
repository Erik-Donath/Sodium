#include "pic.h"
#include "../io.h"
#include "../cpu/isr.h"
#include <stdint.h>
#include <stdio.h>

#define IDT_MASTER_VECTOR 0x20
#define IDT_SLAVE_VECTOR  0x28

#define MASTER_CMD_REG  0x0020
#define MASTER_DATA_REG 0x0021
#define SLAVE_CMD_REG   0x00A0
#define SLAVE_DATA_REG  0x00A1

#define master_command(cmd) i686_io_outb(MASTER_CMD_REG,  (cmd))
#define master_data(cmd)    i686_io_outb(MASTER_DATA_REG, (cmd))
#define slave_command(cmd)  i686_io_outb(SLAVE_CMD_REG,   (cmd))
#define slave_data(cmd)     i686_io_outb(SLAVE_DATA_REG,  (cmd))
#define wait()              i686_io_wait()

enum ICW1 : uint8_t {
    ICW1_ICW4       = 0x01,
    ICW1_SINGLE     = 0x02,
    ICW1_INTERVAL4  = 0x04,
    ICW1_LEVEL      = 0x08,
    ICW1_INITIALIZE = 0x10,
};

#define MASTER_SLAVE_IRQ 0x04
#define SLAVE_CASCADE_ID 0x02

enum ICW4 : uint8_t {
    ICW4_8086           = 0x01,
    ICW4_AUTO_EOI       = 0x02,
    ICW4_BUFFER_MASTER  = 0x04,
    ICW4_BUFFER_SLAVE   = 0x00,
    ICW4_BUFFERRED      = 0x08,
    ICW4_SFNM           = 0x10,
};

enum CMD : uint8_t {
    CMD_INITIALIZE                  = ICW1_INITIALIZE,
    CMD_END_OF_INTERRUPT            = 0x20,
    CMD_SPESIFIC_END_OF_INTERRUPT   = 0x60,
    CMD_READ_IRR                    = 0x0A,
    CMD_READ_ISR                    = 0x0B,
};

static void i8259A_isr_default_handler(i686_isr_cpu_state_t* state) {
    uint8_t int_num = state->int_num;
    bool pic = int_num >= IDT_SLAVE_VECTOR;
    uint8_t port = !pic ? (int_num - IDT_MASTER_VECTOR) : (int_num - IDT_SLAVE_VECTOR);
    
    printf("Unsuported PIC Interrupt %d on %d:%d\n", int_num, (uint8_t)pic + 1, port + 1);
    i8259A_send_eoi((uint8_t)(int_num - IDT_MASTER_VECTOR));
}

void i8259A_enable(void) {
    // 1. Send Initialize (CW1)
    master_command(ICW1_INITIALIZE | ICW1_ICW4);
    slave_command(ICW1_INITIALIZE | ICW1_ICW4);
    wait();

    // 2. Send Vektores (CW2)
    master_data(IDT_MASTER_VECTOR);
    slave_data(IDT_SLAVE_VECTOR);
    wait();

    // 3. Setup Cascade (CW3)
    master_data(MASTER_SLAVE_IRQ);
    slave_data(SLAVE_CASCADE_ID);
    wait();

    // 4. Mode (CW4)
    master_data(ICW4_8086);
    slave_data(ICW4_8086);
    wait();

    // Set default handler
    for(uint8_t int_num = 0x20; int_num < 0x30; int_num++) {
        i686_isr_set_handler(int_num, i8259A_isr_default_handler);
    }

    // Enable all interrupts
    master_data(0x00);
    slave_data(0x00);
    wait();
}

void i8259A_disable(void) {
    master_data(0xFF);
    slave_data(0xFF);
}

void i8259A_send_eoi(uint8_t pin) {
    if(pin >= 8) {
        slave_command(CMD_SPESIFIC_END_OF_INTERRUPT | (pin - 8));
        master_command(CMD_SPESIFIC_END_OF_INTERRUPT | 2);
    }
    else {
        master_command(CMD_SPESIFIC_END_OF_INTERRUPT | pin);
    }
    wait();
}