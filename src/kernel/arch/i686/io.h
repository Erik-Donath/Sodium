#pragma once

#include <stdint.h>
#include <kernel/util/assembly.h>

/*
    Port Belegung:
    See also: https://bochs.sourceforge.io/techspec/PORTS.LST
    See also: https://wiki.osdev.org/I/O_Ports

    0x0000-0x001F	The first legacy DMA controller, often used for transfers to floppies.
    0x0020-0x0021	The first Programmable Interrupt Controller
    0x0022-0x0023	Access to the Model-Specific Registers of Cyrix processors.
    0x0040-0x0047	The PIT (Programmable Interval Timer)
    0x0060-0x0064	The "8042" PS/2 Controller or its predecessors, dealing with keyboards and mice.
    0x0070-0x0071	The CMOS and RTC registers
    0x0080-0x008F	The DMA (Page registers)
    0x0092	        The location of the fast A20 gate register
    0x00A0-0x00A1	The second PIC
    0x00C0-0x00DF	The second DMA controller, often used for soundblasters
    0x00E9	        Home of the Port E9 Hack. Used on some emulators to directly send text to the hosts' console.
    0x0170-0x0177	The secondary ATA harddisk controller.
    0x01F0-0x01F7	The primary ATA harddisk controller.
    0x0278-0x027A	Parallel port
    0x02F8-0x02FF	Second serial port
    0x03B0-0x03DF	The range used for the IBM VGA, its direct predecessors, as well as any modern video card in legacy mode.
    0x03F0-0x03F7	Floppy disk controller
    0x03F8-0x03FF	First serial port
*/

// Used by: E9 Hack
#define E9_HACK                 0xE9  // Out port for debuging

// Used by: VGA Controller
#define VGA_CRTC_INDEX_PORT     0x3D4  // CRT Controller Index Port
#define VGA_CRTC_DATA_PORT      0x3D5  // CRT Controller Data Port

// Used by: PIC Controllers
#define PIC1_COMMAND_PORT       0x20
#define PIC1_DATA_PORT          0x21
#define PIC2_COMMAND_PORT       0xA0
#define PIC2_DATA_PORT          0xA1

// User by: iowait
#define IO_UNUSED_PORT         0x0080

// ? Maybe rewrite as i686_XYZ
void ASMCALL outb(uint16_t port, uint8_t value);
uint8_t ASMCALL inb(uint16_t port);

void ASMCALL EnableInterrupts();
void ASMCALL DisableInterrupts();

void iowait();
