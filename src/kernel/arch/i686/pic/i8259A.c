#include "../ports.h"
#include "i8259A.h"

/*
Layout: 
| Pin Group               | Pins      | Description                                                                           |
|-------------------------|-----------|---------------------------------------------------------------------------------------|
| Interrupt Request Lines | IR0-IR7   | 8 input lines for interrupt requests from external devices                            |
| Data Bus                | D0-D7     | Bidirectional 8-bit data bus for communication with the CPU                           |
| Interrupt Control       | INT, INTA | INT: Output to CPU interrupt input; INTA: Input for interrupt acknowledgment from CPU |
| Chip Select             | CS        | Enables the chip for read/write operations                                            |
| Read/Write Control      | RD, WR    | Control read and write operations                                                     |
| Address Line            | A0        | Used to distinguish between various command and status words                          |
| Cascade Lines           | CAS0-CAS2 | Allow cascading of multiple 8259A chips                                               |
| Slave Program           | SP/EN     | Configures the chip as master or slave in a cascaded configuration                    |
*/

#define PIC1_COMMAND_PORT       0x20
#define PIC1_DATA_PORT          0x21
#define PIC2_COMMAND_PORT       0xA0
#define PIC2_DATA_PORT          0xA1

#define IDT_OFFSET_PIC1         0x20
#define IDT_OFFSET_PIC2         0x28

enum {
    ICW1_ICW4       = 0x01,      // ICW4 needed
    ICW1_SINGLE     = 0x02,      // Single mode
    ICW1_INTERVAL4  = 0x04,      // Call address interval 4
    ICW1_LEVEL      = 0x08,      // Level triggered mode
    ICW1_INITIALIZE = 0x10,      // Initialization required
};

enum {
    ICW4_8086           = 0x01,  // 8086/88 mode
    ICW4_AUTO_EOI       = 0x02,  // Auto EOI
    ICW4_BUFFER_MASTER  = 0x04,  // Buffered mode/master
    ICW4_BUFFER_SLAVE   = 0x00,  // Buffered mode/slave
    ICW4_BUFFERRED      = 0x08,  // Buffered mode
    ICW4_SFNM           = 0x10, // Special fully nested mode
};

typedef enum {
    CMD_END_OF_INTERRUPT            = 0x20,
    CMD_SPESIFIC_END_OF_INTERRUPT   = 0x60,
    CMD_READ_IRR                    = 0x0A,
    CMD_READ_ISR                    = 0x0B,
};

bool i8259A_Check() {
    return true;
}

void i8259A_Enable() {

    // 1. initialization control word 1 - init
    outb(PIC1_COMMAND_PORT, ICW1_ICW4 | ICW1_INITIALIZE);
    outb(PIC2_COMMAND_PORT, ICW1_ICW4 | ICW1_INITIALIZE);
    iowait();

    // 2. initialization control word 2 - offsets
    outb(PIC1_DATA_PORT, IDT_OFFSET_PIC1);
    outb(PIC2_DATA_PORT, IDT_OFFSET_PIC2);
    iowait();

    // 3. initialization control word 3 - master & slave
    outb(PIC1_DATA_PORT, 0x4);  // PIC1 has a slave at IRQ2 (0000 0100)
    outb(PIC2_DATA_PORT, 0x2);  // PIC2 set to cascade identity (0000 0010)
    iowait();

    // 4. initialization control word 4 - mode
    outb(PIC1_DATA_PORT, ICW4_8086);
    outb(PIC2_DATA_PORT, ICW4_8086);
    iowait();

    // 5. Enable all interrupts
    outb(PIC1_DATA_PORT, 0x00);
    outb(PIC2_DATA_PORT, 0x00);
    iowait();

}

void i8259A_Disable() {

    // Disable all interrupts
    outb(PIC1_DATA_PORT, 0xFF);
    outb(PIC2_DATA_PORT, 0xFF);
    iowait();
}

void i8259A_Mask(uint8_t irq) {

}

void i8259A_Unmask( uint8_t irq) {

}

uint16_t i8259A_ReadMask() {
    return 0;
}

uint16_t i8259A_ReadIRQRequestRegister() {
    return 0;
}

uint16_t i8259A_ReadIRQServiceRegister() {
    return 0;
}

void i8259A_SendEOI(uint8_t irq) {

}

void i8259A_SendSEOI(uint8_t irq) {

}
