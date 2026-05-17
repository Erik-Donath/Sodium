#include "pre_kernel.h"
#include <stddef.h>
#include <stdint.h>

#include "debug.h"

// CPU Setup
#include "cpu/fpu.h"
#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "cpu/isr.h"
#include "cpu/tss.h"

// Important Systems
#include "mb2/mb2.h"
#include "mem/memory.h"

// PIC
#include "pic/i8259A.h"
#include "ps2/8042.h"

// VGA
#include "vga/text.h"

// Sound Test
#include "sound/pc_speaker.h"

// Kernel
#include <kernel/kernel.h>

static void i686_timer(i686_isr_cpu_state_t *state) {
  i686_i8259A_send_eoi(state->int_num - i686_i8259A_irq_master);
}

void __attribute__((cdecl)) i686_pre_kernel(i686_mb2_header_t* mb_info) {
  // Seting up the CPU
  i686_gdt_init();
  i686_debug_puts("[OK] GDT initialized\n");
  i686_tss_init();
  i686_debug_puts("[OK] TSS initialized\n");
  i686_gdt_load();
  i686_debug_puts("[OK] GDT loaded\n");
  i686_tss_load();
  i686_debug_puts("[OK] TSS loaded\n");
  i686_fpu_init();
  i686_debug_puts("[OK] FPU initialized\n");

  // Read Multiboot2 Info Struct
  i686_debug_puts("[INFO] Trying to read MB2 info\n");
  if(i686_mb2_parse(mb_info)) {
    i686_debug_puts("[OK] MB2 Info initialized\n");
  }
  else {
    i686_debug_puts("[ERR] Failed to parse MB2 Info\n");
    return;
  }

  // Setup Memory Managment
  const i686_mem_info_t* info = i686_mb2_get_mem_info();
  i686_mem_init(info);
  i686_debug_puts("[OK] Memory Managment initialized\n");

  i686_io_disable_interrupts();
  i686_idt_init();
  i686_debug_puts("[OK] IDT initialized\n");
  i686_isr_init();
  i686_debug_puts("[OK] ISR initialized\n");
  i686_idt_load();
  i686_debug_puts("[OK] IDT loaded\n");

  // #FIXME: Assuming that there is an i8259A controller. Implement a real check
  // against ACPIMADT when implementing APCI driver
  i686_debug_puts("[OK] Found Interrupt Controller: i8259A\n");
  i686_i8259A_enable();
  i686_debug_puts("[OK] Enabled PIC i8259A\n");

  i686_isr_clear_handler(i686_i8259A_irq_master + 0);
  if(i686_isr_set_handler(i686_i8259A_irq_master + 0, i686_timer))
    i686_debug_puts("[OK] Timer initialized\n");
  else {
    i686_debug_puts("[ERR] Failed to setup Timer\n");
    return;
  }

  i686_io_enable_interrupts();
  i686_debug_puts("[OK] IDT Interrupts enabled\n");

  // PS2 Controller; #FIXME: This will be moved into a centrale Driver Managment in the higher Kernel. It is currently here for testing only!!!
  i686_debug_puts("[INFO] Trying to initialize 8042 PS2 Controller\n");
  if(i686_8042_init()) {
    i686_debug_puts("[OK] 8042 PS2 Controller initialized\n");
  }
  else {
    i686_debug_puts("[ERR] Failed to initialize 8042 PS2 Controller\n");
    return;
  }

  // Initialize VGA driver
  i686_vga_text_init(i686_VGA_TEXT_MODE_80x25);
  i686_vga_text_clear(i686_vga_text_make_attr(i686_VGA_COLOR_WHITE, i686_VGA_COLOR_BLACK));
  i686_debug_puts("[OK] VGA initialized\n");

  // Play a Beep at Boot finish
  i686_pcspeaker_stop_sound();
  i686_pcspeaker_beep();

  // Launch high Kernel
  k_main();
}
