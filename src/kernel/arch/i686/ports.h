#pragma once
#include <stdint.h>
#include <kernel/util.h>

#define outb(port, value) i686_PORTS_outb(port, value)
#define inb(port) i686_PORTS_inb(port)
extern void ASM_CALL i686_PORTS_outb(uint16_t port, uint8_t value);
extern uint8_t ASM_CALL i686_PORTS_inb(uint16_t port);

#define outw(port, value) i686_PORTS_outw(port, value)
#define inw(port) i686_PORTS_inw(port)
extern void ASM_CALL i686_PORTS_outw(uint16_t port, uint16_t value);
extern uint16_t ASM_CALL i686_PORTS_inw(uint16_t port);

#define outl(port, value) i686_PORTS_outl(port, value)
#define inl(port) i686_PORTS_inl(port)
extern void ASM_CALL i686_PORTS_outl(uint16_t port, uint32_t value);
extern uint32_t ASM_CALL i686_PORTS_inl(uint16_t port);
