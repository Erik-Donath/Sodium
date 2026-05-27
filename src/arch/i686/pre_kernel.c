#include "pre_kernel.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "cpu/io.h"
#include "cpu/fpu.h"
#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "cpu/isr.h"
#include "cpu/tss.h"
#include "mb2/mb2.h"
#include "mem/pmm.h"
#include "pic/i8259A.h"
#include "ps2/8042.h"
#include "vga/text.h"
#include "sound/pc_speaker.h"

#include <kernel/kernel.h>
#include <kernel/memory/heap.h>

// Internal

// @brief IRQ 0 stub: acknowledges the timer interrupt without processing.
//        Placeholder until the scheduler tick is implemented.
static void i686_timer_irq(i686_isr_cpu_state_t *state);

// Definitions

static void i686_timer_irq(i686_isr_cpu_state_t *state) {
    i686_i8259A_send_eoi(state->int_num - I686_I8259A_IRQ_MASTER);
}

void __attribute__((cdecl)) i686_pre_kernel(i686_mb2_header_t *mb_info) {
    // CPU tables
    i686_gdt_init();  printf("[OK] GDT initialized\n");
    i686_tss_init();  printf("[OK] TSS initialized\n");
    i686_gdt_load();  printf("[OK] GDT loaded\n");
    i686_tss_load();  printf("[OK] TSS loaded\n");
    i686_fpu_init();  printf("[OK] FPU initialized\n");

    // Multiboot2
    printf("[INFO] Trying to read MB2 info\n");
    if (!i686_mb2_parse(mb_info)) {
        printf("[ERR] Failed to parse MB2 Info\n");
        return;
    }
    printf("[OK] MB2 Info initialized\n");

    // Physical memory
    printf("[INFO] Initializing PMM\n");
    const i686_mem_info_t *info = i686_mb2_get_mem_info();
    if (!i686_mem_pmm_init(info)) {
        printf("[ERR] PMM initialization failed\n");
        return;
    }
    printf("[OK] PMM initialized\n");
    mb_info = NULL;  // MB2 struct lives in unprotected memory; discard pointer.

    // Kernel heap
    k_heap_init();
    printf("[OK] Kernel Heap initialized\n");

    // Interrupts
    i686_io_disable_interrupts();
    i686_idt_init();  printf("[OK] IDT initialized\n");
    i686_isr_init();  printf("[OK] ISR initialized\n");
    i686_idt_load();  printf("[OK] IDT loaded\n");

    // TODO: detect interrupt controller from ACPI MADT instead of assuming i8259A.
    printf("[OK] Found Interrupt Controller: i8259A\n");
    i686_i8259A_enable();
    printf("[OK] Enabled PIC i8259A\n");

    i686_isr_clear_handler(I686_I8259A_IRQ_MASTER + 0);
    if (!i686_isr_set_handler(I686_I8259A_IRQ_MASTER + 0, i686_timer_irq)) {
        printf("[ERR] Failed to setup Timer\n");
        return;
    }
    printf("[OK] Timer initialized\n");

    i686_io_enable_interrupts();
    printf("[OK] IDT Interrupts enabled\n");

    // PS/2
    // TODO: move to the Driver Management System in the high kernel.
    printf("[INFO] Trying to initialize 8042 PS2 Controller\n");
    if (!i686_8042_init()) {
        printf("[ERR] Failed to initialize 8042 PS2 Controller\n");
        return;
    }
    printf("[OK] 8042 PS2 Controller initialized\n");

    // VGA
    i686_vga_text_init(I686_VGA_TEXT_MODE_80x25);
    i686_vga_text_clear(I686_VGA_TEXT_MAKE_ATTR(
        I686_VGA_COLOR_WHITE, I686_VGA_COLOR_BLACK));
    printf("[OK] VGA initialized\n");

    // Boot beep
    i686_pcspeaker_stop_sound();
    i686_pcspeaker_beep();

    k_main();
}
