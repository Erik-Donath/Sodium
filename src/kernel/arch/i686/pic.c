#include "io.h"
#include "pic.h"

enum {
    PIC_ICW1_ICW4       = 0x1,
    PIC_ICW1_SINGLE     = 0x2,
    PIC_ICW1_INTERVAL4  = 0x4,
    PIC_ICW1_LEVEL      = 0x8,
    PIC_ICW1_INITIALIZE = 0x10,
} PIC_ICW1;

enum {
    PIC_ICW4_8086           = 0x1,
    PIC_ICW4_AUTO_EOI       = 0x2,
    PIC_ICW4_BUFFER_MASTER  = 0x4,
    PIC_ICW4_BUFFER_SLAVE   = 0x0,
    PIC_ICW4_BUFFERRED      = 0x8,
    PIC_ICW4_SFNM           = 0x10,
} PIC_ICW4;

enum {
    PIC_CMD_END_OF_INTERRUPT            = 0x20,
    PIC_CMD_SPESIFIC_END_OF_INTERRUPT   = 0x60,
    PIC_CMD_READ_IRR                    = 0x0A,
    PIC_CMD_READ_ISR                    = 0x0B,
} PIC_CMD;

void PIC_Configure(uint8_t offsetPic1, uint8_t offsetPic2) {

    // initialization control word 1 - init
    outb(PIC1_COMMAND_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INITIALIZE);
    outb(PIC2_COMMAND_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INITIALIZE);
    iowait();
    
    // initialization control word 2 - offsets
    outb(PIC1_DATA_PORT, offsetPic1);
    outb(PIC2_DATA_PORT, offsetPic2);
    iowait();

    // initialization control word 3 - slave & master
    outb(PIC1_DATA_PORT, 0x4);  // PIC1 has a slave at IRQ2 (0000 0100)
    outb(PIC2_DATA_PORT, 0x2);  // PIC2 set to cascade identity (0000 0010)
    iowait();

    // initialization control word 4 - mode
    outb(PIC1_DATA_PORT, PIC_ICW4_8086);
    outb(PIC2_DATA_PORT, PIC_ICW4_8086);
    iowait();

    // enable all interrupts
    outb(PIC1_DATA_PORT, 0x00);
    outb(PIC2_DATA_PORT, 0x00);
    iowait();
}

void PIC_Disable() {
    outb(PIC1_DATA_PORT, 0xFF);
    outb(PIC2_DATA_PORT, 0xFF);
    iowait();
}

void PIC_Mask(uint8_t irq) {
    uint8_t port;
    if(irq < 8)
        port = PIC1_DATA_PORT;
    else {
        irq -= 8;
        port = PIC2_DATA_PORT;
    }

    uint8_t mask = inb(port);
    outb(port, mask | (1 << irq));
}

void PIC_Unmask(uint8_t irq) {
    uint8_t port;
    if(irq < 8)
        port = PIC1_DATA_PORT;
    else {
        irq -= 8;
        port = PIC2_DATA_PORT;
    }

    uint8_t mask = inb(port);
    outb(port, mask & ~(1 << irq));
}

uint16_t PIC_ReadMask() {
    return inb(PIC1_DATA_PORT) | (inb(PIC2_DATA_PORT) << 8);
}

uint16_t PIC_ReadIRQRequestRegister() {
    outb(PIC1_COMMAND_PORT, PIC_CMD_READ_IRR);
    outb(PIC2_COMMAND_PORT, PIC_CMD_READ_IRR);
    return inb(PIC1_DATA_PORT) | (inb(PIC2_DATA_PORT) << 8);
}

uint16_t PIC_ReadIRQServicRegister() {
    outb(PIC1_COMMAND_PORT, PIC_CMD_READ_ISR);
    outb(PIC2_COMMAND_PORT, PIC_CMD_READ_ISR);
    return inb(PIC1_DATA_PORT) | (inb(PIC2_DATA_PORT) << 8);
}

void PIC_SendEOI(uint8_t irq) {
    if(irq >= 8)
        outb(PIC2_COMMAND_PORT, PIC_CMD_END_OF_INTERRUPT);
    outb(PIC1_COMMAND_PORT, PIC_CMD_END_OF_INTERRUPT);
}

void PIC_SendSEOI(uint8_t irq) {
    if(irq >= 8) {
        outb(PIC2_COMMAND_PORT, PIC_CMD_SPESIFIC_END_OF_INTERRUPT + (irq - 8));
        outb(PIC1_COMMAND_PORT, PIC_CMD_SPESIFIC_END_OF_INTERRUPT + 2);
    }
    else {
        outb(PIC1_COMMAND_PORT, PIC_CMD_SPESIFIC_END_OF_INTERRUPT + irq);
    }
}
