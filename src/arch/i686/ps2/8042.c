// Intel 8042 PS/2 Controller Driver
// https://wiki.osdev.org/8042_PS/2_Controller
// TODO: integrate into the Driver Management System as a bus driver.

#include "8042.h"
#include "ps2_MF2_keyboard.h"
#include "ps2_mouse.h"
#include "../pic/i8259A.h"
#include "../cpu/isr.h"
#include "../cpu/io.h"

#include <stdio.h>
#include <stddef.h>

// Internal

#define DATA_PORT    0x60
#define STATUS_PORT  0x64
#define CMD_PORT     0x64

#define STATUS_OBF         0x01
#define STATUS_IBF         0x02
#define STATUS_PORT_B_OBF  0x20

#define CTRL_CFG_READ        0x20
#define CTRL_CFG_WRITE       0x60
#define CTRL_PORT_B_DISABLE  0xA7
#define CTRL_PORT_B_ENABLE   0xA8
#define CTRL_PORT_B_TEST     0xA9
#define CTRL_SELF_TEST       0xAA
#define CTRL_PORT_A_TEST     0xAB
#define CTRL_PORT_A_DISABLE  0xAD
#define CTRL_PORT_A_ENABLE   0xAE
#define CTRL_PORT_B_WRITE    0xD4

#define CFG_PORT_A_IRQ   0x01
#define CFG_PORT_B_IRQ   0x02
#define CFG_PORT_B_CLK   0x20
#define CFG_TRANSLATION  0x40

#define DEV_RESET         0xFF
#define DEV_IDENTIFY      0xF2
#define DEV_DISABLE_SCAN  0xF5
#define DEV_ACK           0xFA
#define DEV_BAT_OK        0xAA

#define IRQ_PORT_A      1
#define IRQ_PORT_B      12

#define TIMEOUT         100000u
#define FLUSH_MAX       16
#define DRIVER_COUNT    2

#define WAIT_PORT(cond) ({                               \
    i686_8042_result_t _r = i686_8042_ERR_TIMEOUT;       \
    for (uint32_t _i = 0; _i < TIMEOUT; _i++) {          \
        if (cond) { _r = i686_8042_OK; break; }          \
        i686_io_wait();                                  \
    }                                                    \
    _r;                                                  \
})

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

static const i686_8042_driver_t *const drivers[DRIVER_COUNT] = {
    &i686_ps2_mf2_kbd_driver,
    &i686_ps2_mouse_driver,
};

static i686_8042_port_state_t ports[I686_8042_PORT_COUNT];

// #FIXME: These Functions are declared twice
static void i686_8042_irq_port_a(i686_isr_cpu_state_t *state);
static void i686_8042_irq_port_b(i686_isr_cpu_state_t *state);

static const i686_8042_port_ctrl_t ctrl[I686_8042_PORT_COUNT] = {
    [I686_8042_PORT_A] = {
        .enable      = CTRL_PORT_A_ENABLE,
        .disable     = CTRL_PORT_A_DISABLE,
        .test        = CTRL_PORT_A_TEST,
        .cfg_irq     = CFG_PORT_A_IRQ,
        .irq_pin     = IRQ_PORT_A,
        .irq_vec     = I686_I8259A_IRQ_MASTER + IRQ_PORT_A,
        .irq_handler = i686_8042_irq_port_a,
    },
    [I686_8042_PORT_B] = {
        .enable      = CTRL_PORT_B_ENABLE,
        .disable     = CTRL_PORT_B_DISABLE,
        .test        = CTRL_PORT_B_TEST,
        .cfg_irq     = CFG_PORT_B_IRQ,
        .irq_pin     = IRQ_PORT_B,
        .irq_vec     = I686_I8259A_IRQ_SLAVE + (IRQ_PORT_B - I686_I8259A_IRQ_SIZE),
        .irq_handler = i686_8042_irq_port_b,
    },
};

// @brief Block until the controller input buffer is empty, or timeout.
static i686_8042_result_t i686_8042_wait_write(void);

// @brief Block until the controller output buffer has data, or timeout.
static i686_8042_result_t i686_8042_wait_read(void);

// @brief Send a command byte to the controller command port (0x64).
static i686_8042_result_t i686_8042_ctrl_cmd(uint8_t c);

// @brief Write one data byte to the PS/2 data port (0x60).
static i686_8042_result_t i686_8042_data_send(uint8_t d);

// @brief Read one data byte from the PS/2 data port.
// @param out  Receives the byte.
static i686_8042_result_t i686_8042_data_recv(uint8_t *out);

// @brief Drain up to FLUSH_MAX stale bytes from the output buffer.
static void i686_8042_flush(void);

// @brief Read the controller configuration byte.
// @param out  Receives the byte on success.
static i686_8042_result_t i686_8042_cfg_read(uint8_t *out);

// @brief Write the controller configuration byte.
static i686_8042_result_t i686_8042_cfg_write(uint8_t val);

// @brief Enable IRQ for a port; unmask its PIC pin.
static void i686_8042_port_irq_enable(uint8_t port);

// @brief Mask PIC pin; clear ISR handler; disable cfg IRQ bit.
static void i686_8042_port_irq_disable(uint8_t port);

// @brief Read a byte from whichever port the status byte indicates.
//        Dispatches to the owning driver's data callback.
static void i686_8042_irq_dispatch(void);

// @brief IRQ handler for PS/2 port A (IRQ 1, vector 33).
static void i686_8042_irq_port_a(i686_isr_cpu_state_t *state);

// @brief IRQ handler for PS/2 port B (IRQ 12, vector 44).
static void i686_8042_irq_port_b(i686_isr_cpu_state_t *state);

// @brief Find a registered driver matching device_id.
// @return Pointer to driver, or NULL.
static const i686_8042_driver_t *i686_8042_find_driver(i686_ps2_device_t id);

// @brief Reset a device and read its 1- or 2-byte identification code.
//        Returns I686_PS2_DEV_AT_KBD if no ID bytes arrive (legacy keyboard).
//        Returns 0xFFFF on any communication failure.
// @param port  Port index to probe.
static uint16_t i686_8042_identify(uint8_t port);

// @brief Enable a port, identify its device, load a driver, enable its IRQ.
//        Disables the port and logs a warning if anything fails.
static void i686_8042_discover_port(uint8_t port);

// Definitions

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
    if (i686_8042_ctrl_cmd(CTRL_CFG_READ) != i686_8042_OK)
        return i686_8042_ERR_TIMEOUT;
    return i686_8042_data_recv(out);
}

static i686_8042_result_t i686_8042_cfg_write(uint8_t val) {
    if (i686_8042_ctrl_cmd(CTRL_CFG_WRITE) != i686_8042_OK)
        return i686_8042_ERR_TIMEOUT;
    return i686_8042_data_send(val);
}

static void i686_8042_port_irq_enable(uint8_t port) {
    uint8_t cfg;
    if (i686_8042_cfg_read(&cfg) != i686_8042_OK) return;
    cfg |= ctrl[port].cfg_irq;
    i686_8042_cfg_write(cfg);
    i686_isr_set_handler(ctrl[port].irq_vec, ctrl[port].irq_handler);
    i686_i8259A_unmask(ctrl[port].irq_pin);
}

static void i686_8042_port_irq_disable(uint8_t port) {
    i686_i8259A_mask(ctrl[port].irq_pin);
    i686_isr_clear_handler(ctrl[port].irq_vec);
    uint8_t cfg;
    if (i686_8042_cfg_read(&cfg) != i686_8042_OK) return;
    cfg &= (uint8_t)~ctrl[port].cfg_irq;
    i686_8042_cfg_write(cfg);
}

static void i686_8042_irq_dispatch(void) {
    uint8_t status = i686_io_inb(STATUS_PORT);
    uint8_t data   = i686_io_inb(DATA_PORT);
    // STATUS_PORT_B_OBF can be set even during PORT_A's IRQ on some chipsets.
    uint8_t port = (status & STATUS_PORT_B_OBF)
                   ? I686_8042_PORT_B : I686_8042_PORT_A;
    i686_8042_port_state_t *ps = &ports[port];
    if (ps->driver) ps->driver->data(port, ps->ctx, data);
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

static const i686_8042_driver_t *i686_8042_find_driver(i686_ps2_device_t id) {
    for (uint8_t i = 0; i < DRIVER_COUNT; i++)
        if (drivers[i]->device_id == id) return drivers[i];
    return NULL;
}

static uint16_t i686_8042_identify(uint8_t port) {
    uint8_t resp;

    if (i686_8042_write(port, DEV_RESET) != i686_8042_OK) return 0xFFFF;
    if (i686_8042_data_recv(&resp)       != i686_8042_OK) return 0xFFFF;
    if (resp != DEV_ACK)                                  return 0xFFFF;
    if (i686_8042_data_recv(&resp)       != i686_8042_OK) return 0xFFFF;
    if (resp != DEV_BAT_OK)                               return 0xFFFF;

    i686_8042_flush();

    (void)i686_8042_write(port, DEV_DISABLE_SCAN);
    if (i686_8042_data_recv(&resp) != i686_8042_OK || resp != DEV_ACK)
        i686_8042_flush();

    if (i686_8042_write(port, DEV_IDENTIFY) != i686_8042_OK) return 0xFFFF;
    if (i686_8042_data_recv(&resp)          != i686_8042_OK) return 0xFFFF;
    if (resp != DEV_ACK)                                     return 0xFFFF;

    uint8_t id0;
    if (i686_8042_data_recv(&id0) != i686_8042_OK)
        return I686_PS2_DEV_AT_KBD;

    uint8_t id1;
    if (i686_8042_data_recv(&id1) != i686_8042_OK)
        return (uint16_t)id0;

    return (uint16_t)((id0 << 8) | id1);
}

static void i686_8042_discover_port(uint8_t port) {
    if (i686_8042_ctrl_cmd(ctrl[port].enable) != i686_8042_OK) return;

    uint16_t dev_id = i686_8042_identify(port);
    printf("[INFO] Discovered Device %X at PS2-Port %d\n", dev_id, port);

    if (dev_id == 0xFFFF) {
        i686_8042_ctrl_cmd(ctrl[port].disable);
        printf("[WARN] Device on PS2-Port %d is faulty\n", port);
        return;
    }

    const i686_8042_driver_t *drv =
        i686_8042_find_driver((i686_ps2_device_t)dev_id);
    if (!drv) {
        i686_8042_ctrl_cmd(ctrl[port].disable);
        printf("[WARN] No matching driver found for Device %X on PS2-Port %d\n",
               dev_id, port);
        return;
    }

    ports[port].driver = drv;
    ports[port].ctx    = drv->open(port);
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

    if (i686_8042_cfg_write(cfg) != i686_8042_OK) goto fail;

    if (i686_8042_ctrl_cmd(CTRL_PORT_A_TEST) != i686_8042_OK) goto fail;
    if (i686_8042_data_recv(&resp)           != i686_8042_OK) goto fail;
    ports[I686_8042_PORT_A].exists = (resp == 0x00);

    if (i686_8042_ctrl_cmd(CTRL_PORT_B_ENABLE)  != i686_8042_OK) goto fail;
    if (i686_8042_cfg_read(&cfg)                != i686_8042_OK) goto fail;
    bool has_port_b = !(cfg & CFG_PORT_B_CLK);
    if (i686_8042_ctrl_cmd(CTRL_PORT_B_DISABLE) != i686_8042_OK) goto fail;

    if (has_port_b) {
        if (i686_8042_ctrl_cmd(CTRL_PORT_B_TEST) != i686_8042_OK) goto fail;
        if (i686_8042_data_recv(&resp)           != i686_8042_OK) goto fail;
        ports[I686_8042_PORT_B].exists = (resp == 0x00);
    }

    for (uint8_t port = 0; port < I686_8042_PORT_COUNT; port++)
        if (ports[port].exists)
            i686_8042_discover_port(port);

    return true;

fail:
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

    i686_8042_port_irq_disable(port);
    i686_8042_ctrl_cmd(ctrl[port].disable);

    if (ports[port].driver)
        ports[port].driver->close(port, ports[port].ctx);

    ports[port].driver = NULL;
    ports[port].ctx    = NULL;
    return i686_8042_OK;
}

i686_8042_result_t i686_8042_reset_port(uint8_t port) {
    if (port >= I686_8042_PORT_COUNT) return i686_8042_ERR_BAD_PORT;
    i686_8042_result_t r = i686_8042_close(port);
    if (r != i686_8042_OK) return r;
    if (ports[port].exists) i686_8042_discover_port(port);
    return i686_8042_OK;
}
