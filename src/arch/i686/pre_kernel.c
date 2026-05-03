#include "pre_kernel.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "debug.h"

// CPU Setup
#include "cpu/fpu.h"
#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "cpu/isr.h"
#include "cpu/tss.h"

// PIC
#include "pic/i8259A.h"

// VGA
#include "vga/text.h"

static void i686_timer(i686_isr_cpu_state_t *state) {
  i686_i8259A_send_eoi(state->int_num - i686_i8259A_irq_master);
}

void __attribute__((cdecl)) i686_pre_kernel(void *mb_info) {
  (void)mb_info;

  // Seting up the CPU
  i686_gdt_init();
  i686_debug_puts("[OK] GDT Initialized\n");
  i686_tss_init();
  i686_debug_puts("[OK] TSS Initialized\n");
  i686_gdt_load();
  i686_debug_puts("[OK] GDT Loaded\n");
  i686_tss_load();
  i686_debug_puts("[OK] TSS Loaded\n");
  i686_fpu_init();
  i686_debug_puts("[OK] FPU Initialized\n");

  i686_io_disable_interrupts();
  i686_idt_init();
  i686_debug_puts("[OK] IDT Initialized\n");
  i686_isr_init();
  i686_debug_puts("[OK] ISR Initialized\n");
  i686_idt_load();
  i686_debug_puts("[OK] IDT Loaded\n");

  // #FIXME: Assuming that there is an i8259A controller. Implement a real check
  // against ACPIMADT when implementing APCI driver
  i686_debug_puts("[OK] Found Interrupt Controller: i8259A\n");
  i686_i8259A_enable();
  i686_debug_puts("[OK] Enabled PIC i8259A\n");

  i686_isr_clear_handler(i686_i8259A_irq_master + 0);
  if(i686_isr_set_handler(i686_i8259A_irq_master + 0, i686_timer))
    i686_debug_puts("[OK] Timer setup\n");
  else {
    i686_debug_puts("[ERR] Failed to setup Timer\n");
    return;
  }

  i686_io_enable_interrupts();
  i686_debug_puts("[OK] IDT Interrupts Enabled\n");

  // Initialize VGA driver
  i686_vga_text_init(i686_VGA_TEXT_MODE_80x25);
  i686_vga_text_clear(i686_vga_text_make_attr(i686_VGA_COLOR_WHITE, i686_VGA_COLOR_BLACK));
  i686_debug_puts("[OK] VGA Initialized\n");

  const char* hello = "Hello User!";
  for(uint8_t y = 0; y < 50; y++) {
  for(uint8_t x = 0; x < 11; x++) {
    i686_vga_text_put_cell(x, y, i686_vga_text_make_char(hello[x], i686_VGA_COLOR_CYAN, i686_VGA_COLOR_BLACK));
    i686_vga_text_cursor_set_pos(x, y);
  }
}

  // Printing Sodium in aqua using ASCII Escape Seqenz to qemu debug output
  printf("\033[38;5;6;48;5;0mSODIUM\033[0m\n");

  while (1)
    ;

  return;
}
