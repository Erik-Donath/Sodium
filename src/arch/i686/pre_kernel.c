#include "pre_kernel.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "debug.h"

// CPU Setup
#include "cpu/gdt.h"
#include "cpu/tss.h"
#include "cpu/fpu.h"
#include "cpu/idt.h"
#include "cpu/isr.h"

void __attribute__((cdecl)) pre_kernel(void *mb_info) {
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

  // #FIXME: Deaktivate PIC1 and PIC2
  outb(0x21, 0xFF);
  outb(0xA1, 0xFF);

  i686_io_enable_interrupts();
  i686_debug_puts("[OK] IDT Interrupts Enabled\n");

  // Printing Sodium in aqua to VGA Output
  static char *i686_vga = (char *)0xB8000;
  const char *hello = "Sodium            ";
  for (uint32_t i = 0; hello[i]; i++) {
    i686_vga[(i << 1) + 0] = hello[i]; // Char
    i686_vga[(i << 1) + 1] = 0x03;     // Color
  }

  // Printing Sodium in aqua using ASCII Escape Seqenz to qemu debug output
  printf("\033[38;5;6;48;5;0mSODIUM\033[0m\n");

  return;
}
