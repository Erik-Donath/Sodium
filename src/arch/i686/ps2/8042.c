// Intel 8042 PS/2 Controller Driver
// https://wiki.osdev.org/8042_PS/2_Controller
// https://en.wikipedia.org/wiki/Intel_8042
//
// Port B is officaly named the "auxiliary" port. This driver calls it Port B (index 1). If you want to retrive the Hardware name use the STATUS_PORT_B_OBF constance please!
// #FIXME: This driver is temporarly. It will be integrated in the Driver Managment System when the higher Kernel is written as an BUS Driver.

#include "8042.h"
#include "ps2_MF2_keyboard.h"
#include "../pic/i8259A.h"
#include "../cpu/isr.h"
#include "../cpu/io.h"
#include <stdio.h>
#include <stddef.h>

#define DATA_PORT    0x60
#define STATUS_PORT  0x64
#define CMD_PORT     0x64

#define STATUS_OBF        0x01
#define STATUS_IBF        0x02
#define STATUS_PORT_B_OBF 0x20

#define CTRL_CFG_READ       0x20
#define CTRL_CFG_WRITE      0x60
#define CTRL_PORT_B_DISABLE 0xA7
#define CTRL_PORT_B_ENABLE  0xA8
#define CTRL_PORT_B_TEST    0xA9
#define CTRL_SELF_TEST      0xAA
#define CTRL_PORT_A_TEST    0xAB
#define CTRL_PORT_A_DISABLE 0xAD
#define CTRL_PORT_A_ENABLE  0xAE
#define CTRL_PORT_B_WRITE   0xD4

#define CFG_PORT_A_IRQ  0x01
#define CFG_PORT_B_IRQ  0x02
#define CFG_PORT_B_CLK  0x20
#define CFG_TRANSLATION 0x40

#define DEV_RESET        0xFF
#define DEV_IDENTIFY     0xF2
#define DEV_DISABLE_SCAN 0xF5
#define DEV_ACK          0xFA
#define DEV_BAT_OK       0xAA

#define IRQ_PORT_A 1
#define IRQ_PORT_B 12

#define TIMEOUT   100000u
#define FLUSH_MAX 16

#define DRIVER_COUNT 1

#define WAIT_PORT(cond) ({                                   \
    i686_8042_result_t _r = i686_8042_ERR_TIMEOUT;           \
    for (uint32_t _i = 0; _i < TIMEOUT; _i++) {              \
        if (cond) { _r = i686_8042_OK; break; }              \
        i686_io_wait();                                      \
    }                                                        \
    _r;                                                      \
})

static const i686_8042_driver_t * const s_drivers[DRIVER_COUNT] = {
    &i686_ps2_kbd_driver,
};

typedef struct {
    uint8_t            enable;
    uint8_t            disable;
    uint8_t            test;
    uint8_t            cfg_irq;
    uint8_t            irq_pin;
    uint8_t            irq_vec;
    i686_isr_handler_t irq_handler;
} i686_8042_port_ctrl_t;

typedef struct {
    const i686_8042_driver_t *driver;
    void                     *ctx;
    bool                      exists;
} i686_8042_port_state_t;

static i686_8042_port_state_t s_ports[I686_8042_PORT_COUNT];

static void i686_8042_irq_port_a(i686_isr_cpu_state_t *state);
static void i686_8042_irq_port_b(i686_isr_cpu_state_t *state);

static const i686_8042_port_ctrl_t s_ctrl[I686_8042_PORT_COUNT] = {
    [I686_8042_PORT_A] = {
        .enable      = CTRL_PORT_A_ENABLE,
        .disable     = CTRL_PORT_A_DISABLE,
        .test        = CTRL_PORT_A_TEST,
        .cfg_irq     = CFG_PORT_A_IRQ,
        .irq_pin     = IRQ_PORT_A,
        .irq_vec     = i686_i8259A_irq_master + IRQ_PORT_A,
        .irq_handler = i686_8042_irq_port_a,
    },
    [I686_8042_PORT_B] = {
        .enable      = CTRL_PORT_B_ENABLE,
        .disable     = CTRL_PORT_B_DISABLE,
        .test        = CTRL_PORT_B_TEST,
        .cfg_irq     = CFG_PORT_B_IRQ,
        .irq_pin     = IRQ_PORT_B,
        .irq_vec     = i686_i8259A_irq_slave + (IRQ_PORT_B - i686_i8259A_irq_size),
        .irq_handler = i686_8042_irq_port_b,
    },
};

static i686_8042_result_t i686_8042_wait_write(void) {
    return WAIT_PORT(!(i686_io_inb(STATUS_PORT) & STATUS_IBF));
}

static i686_8042_result_t i686_8042_wait_read(void) {
    return WAIT_PORT(i686_io_inb(STATUS_PORT) & STATUS_OBF);
}

static i686_8042_result_t i686_8042_ctrl_cmd(uint8_t c) {
    i686_8042_result_t r = i686_8042_wait_write();
    if (r != i686_8042_OK) return r;
    i686_io_outb(CMD_PORT, c);
    return i686_8042_OK;
}

static i686_8042_result_t i686_8042_data_send(uint8_t d) {
    i686_8042_result_t r = i686_8042_wait_write();
    if (r != i686_8042_OK) return r;
    i686_io_outb(DATA_PORT, d);
    return i686_8042_OK;
}

static i686_8042_result_t i686_8042_data_recv(uint8_t *out) {
    i686_8042_result_t r = i686_8042_wait_read();
    if (r != i686_8042_OK) return r;
    *out = i686_io_inb(DATA_PORT);
    return i686_8042_OK;
}

static void i686_8042_flush(void) {
    for (int i = 0; i < FLUSH_MAX; i++) {
        if (!(i686_io_inb(STATUS_PORT) & STATUS_OBF)) break;
        (void)i686_io_inb(DATA_PORT);
        i686_io_wait();
    }
}

static i686_8042_result_t i686_8042_cfg_read(uint8_t *out) {
    if (i686_8042_ctrl_cmd(CTRL_CFG_READ) != i686_8042_OK) return i686_8042_ERR_TIMEOUT;
    return i686_8042_data_recv(out);
}

static i686_8042_result_t i686_8042_cfg_write(uint8_t val) {
    if (i686_8042_ctrl_cmd(CTRL_CFG_WRITE) != i686_8042_OK) return i686_8042_ERR_TIMEOUT;
    return i686_8042_data_send(val);
}

static void i686_8042_port_irq_enable(uint8_t port) {
    uint8_t cfg;
    if (i686_8042_cfg_read(&cfg) != i686_8042_OK) return;
    cfg |= s_ctrl[port].cfg_irq;
    i686_8042_cfg_write(cfg);
    i686_isr_set_handler(s_ctrl[port].irq_vec, s_ctrl[port].irq_handler);
    i686_i8259A_unmask(s_ctrl[port].irq_pin);
}

static void i686_8042_port_irq_disable(uint8_t port) {
    i686_i8259A_mask(s_ctrl[port].irq_pin);
    i686_isr_clear_handler(s_ctrl[port].irq_vec);
    uint8_t cfg;
    if (i686_8042_cfg_read(&cfg) != i686_8042_OK) return;
    cfg &= (uint8_t)~s_ctrl[port].cfg_irq;
    i686_8042_cfg_write(cfg);
}

static void i686_8042_irq_dispatch(void) {
    uint8_t status = i686_io_inb(STATUS_PORT);
    uint8_t data   = i686_io_inb(DATA_PORT);
    // STATUS_PORT_B_OBF indicates the byte came from PORT_B even if PORT_A's
    // IRQ fired. Some controllers exhibit this quirk.
    uint8_t port = (status & STATUS_PORT_B_OBF) ? I686_8042_PORT_B : I686_8042_PORT_A;
    i686_8042_port_state_t *ps = &s_ports[port];
    if (ps->driver)
        ps->driver->data(port, ps->ctx, data);
}

static void i686_8042_irq_port_a(i686_isr_cpu_state_t *state) {
    (void)state;
    i686_8042_irq_dispatch();
    i686_i8259A_send_eoi(IRQ_PORT_A);
}

static void i686_8042_irq_port_b(i686_isr_cpu_state_t *state) {
    (void)state;
    i686_8042_irq_dispatch();
    i686_i8259A_send_eoi(IRQ_PORT_B);
}

static const i686_8042_driver_t *i686_8042_find_driver(i686_ps2_device_t device_id) {
    for (uint8_t i = 0; i < DRIVER_COUNT; i++) {
        if (s_drivers[i]->device_id == device_id)
            return s_drivers[i];
    }
    return NULL;
}

static uint16_t i686_8042_identify(uint8_t port) {
    uint8_t resp;

    if (i686_8042_write(port, DEV_RESET)  != i686_8042_OK) return 0xFFFF;
    if (i686_8042_data_recv(&resp)        != i686_8042_OK) return 0xFFFF;
    if (resp != DEV_ACK)                                    return 0xFFFF;
    if (i686_8042_data_recv(&resp)        != i686_8042_OK) return 0xFFFF;
    if (resp != DEV_BAT_OK)                                 return 0xFFFF;

    i686_8042_flush();

    // Disable scanning before identify to prevent stray bytes corrupting
    // the ID response. Not all devices ACK 0xF5 cleanly so errors are ignored.
    (void)i686_8042_write(port, DEV_DISABLE_SCAN);
    if (i686_8042_data_recv(&resp) != i686_8042_OK || resp != DEV_ACK)
        i686_8042_flush();

    if (i686_8042_write(port, DEV_IDENTIFY) != i686_8042_OK) return 0xFFFF;
    if (i686_8042_data_recv(&resp)          != i686_8042_OK) return 0xFFFF;
    if (resp != DEV_ACK)                                       return 0xFFFF;

    uint8_t id0;
    if (i686_8042_data_recv(&id0) != i686_8042_OK)
        return I686_PS2_DEV_AT_KBD;

    uint8_t id1;
    if (i686_8042_data_recv(&id1) != i686_8042_OK)
        return (uint16_t)id0;

    return (uint16_t)((id0 << 8) | id1);
}

static void i686_8042_discover_port(uint8_t port) {
    if (i686_8042_ctrl_cmd(s_ctrl[port].enable) != i686_8042_OK) return;

    uint16_t dev_id = i686_8042_identify(port);
    printf("[INFO] Discovered Device %X at PS2-Port %d\n", dev_id, port);
    if (dev_id == 0xFFFF) {
        i686_8042_ctrl_cmd(s_ctrl[port].disable);
        printf("[WARN] Device found on PS2-Port %d is faulty, connection setup aborted\n", port);
        return;
    }

    const i686_8042_driver_t *drv = i686_8042_find_driver((i686_ps2_device_t)dev_id);
    if (!drv) {
        i686_8042_ctrl_cmd(s_ctrl[port].disable);
        printf("[WARN] No matching driver found for Device %X on PS2-Port %d\n", dev_id, port);
        return;
    }

    i686_8042_port_state_t *ps = &s_ports[port];
    ps->driver = drv;
    ps->ctx    = drv->open(port);

    i686_8042_port_irq_enable(port);
}

bool i686_8042_init(void) {
    uint8_t resp, cfg;

    if (i686_8042_ctrl_cmd(CTRL_PORT_A_DISABLE) != i686_8042_OK) goto fail;
    if (i686_8042_ctrl_cmd(CTRL_PORT_B_DISABLE) != i686_8042_OK) goto fail;
    i686_8042_flush();

    if (i686_8042_cfg_read(&cfg) != i686_8042_OK) goto fail;
    cfg &= (uint8_t)~(CFG_PORT_A_IRQ | CFG_PORT_B_IRQ | CFG_TRANSLATION);
    if (i686_8042_cfg_write(cfg) != i686_8042_OK) goto fail;

    if (i686_8042_ctrl_cmd(CTRL_SELF_TEST) != i686_8042_OK) goto fail;
    if (i686_8042_data_recv(&resp)         != i686_8042_OK) goto fail;
    if (resp != 0x55)                                        goto fail;

    // Restore cfg. Note that a self test may reset controller state.
    if (i686_8042_cfg_write(cfg) != i686_8042_OK) goto fail;

    if (i686_8042_ctrl_cmd(CTRL_PORT_A_TEST) != i686_8042_OK) goto fail;
    if (i686_8042_data_recv(&resp)           != i686_8042_OK) goto fail;
    s_ports[I686_8042_PORT_A].exists = (resp == 0x00);

    // Detect PORT_B by re-enabling it and checking if the clock-disable bit clears.
    if (i686_8042_ctrl_cmd(CTRL_PORT_B_ENABLE)  != i686_8042_OK) goto fail;
    if (i686_8042_cfg_read(&cfg)                != i686_8042_OK) goto fail;
    bool has_port_b = !(cfg & CFG_PORT_B_CLK);
    if (i686_8042_ctrl_cmd(CTRL_PORT_B_DISABLE) != i686_8042_OK) goto fail;

    if (has_port_b) {
        if (i686_8042_ctrl_cmd(CTRL_PORT_B_TEST) != i686_8042_OK) goto fail;
        if (i686_8042_data_recv(&resp)           != i686_8042_OK) goto fail;
        s_ports[I686_8042_PORT_B].exists = (resp == 0x00);
    }

    for (uint8_t port = 0; port < I686_8042_PORT_COUNT; port++) {
        if (s_ports[port].exists)
            i686_8042_discover_port(port);
    }

    return true;

fail:
    // Leave the controller with both ports disabled and all IRQs off.
    // The output port (0xD1) is intentionally not written to avoid
    // disturbing the A20 gate or triggering a CPU reset via bit 0.
    // #Note: The goto statement is ONLY used in cleanup sittuation just like this one! It is totaly fine and the cleanest approche here!
    i686_8042_ctrl_cmd(CTRL_PORT_A_DISABLE);
    i686_8042_ctrl_cmd(CTRL_PORT_B_DISABLE);
    i686_8042_cfg_write(0x00);
    return false;
}

i686_8042_result_t i686_8042_write(uint8_t port, uint8_t data) {
    if (port >= I686_8042_PORT_COUNT) return i686_8042_ERR_BAD_PORT;
    if (port == I686_8042_PORT_B) {
        i686_8042_result_t r = i686_8042_ctrl_cmd(CTRL_PORT_B_WRITE);
        if (r != i686_8042_OK) return r;
    }
    return i686_8042_data_send(data);
}

i686_8042_result_t i686_8042_read(uint8_t *out) {
    return i686_8042_data_recv(out);
}

i686_8042_result_t i686_8042_close(uint8_t port) {
    if (port >= I686_8042_PORT_COUNT) return i686_8042_ERR_BAD_PORT;

    i686_8042_port_state_t *ps = &s_ports[port];

    i686_8042_port_irq_disable(port);
    i686_8042_ctrl_cmd(s_ctrl[port].disable);

    if (ps->driver)
        ps->driver->close(port, ps->ctx);

    ps->driver = NULL;
    ps->ctx    = NULL;

    return i686_8042_OK;
}

i686_8042_result_t i686_8042_reset_port(uint8_t port) {
    if (port >= I686_8042_PORT_COUNT) return i686_8042_ERR_BAD_PORT;

    i686_8042_result_t r = i686_8042_close(port);
    if (r != i686_8042_OK) return r;

    if (s_ports[port].exists)
        i686_8042_discover_port(port);

    return i686_8042_OK;
}
