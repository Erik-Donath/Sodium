#include "pre_kernel.h"
#include "io.h"

// CPU Setup
#include "gdt.h"
#include "tss.h"
#include <stdint.h>

void __attribute__((cdecl)) pre_kernel(void *mb_info) {
  (void)mb_info;

  // Seting up the CPU
  i686_gdt_init();
  i686_tss_init();
  i686_gdt_load();
  i686_tss_load();

  // Printing Sodium in aqua to VGA Output
  static char *i686_vga = (char *)0xB8000;
  const char *hello = "Sodium            ";
  for (uint32_t i = 0; hello[i]; i++) {
    i686_vga[(i << 1) + 0] = hello[i]; // Char
    i686_vga[(i << 1) + 1] = 0x03;     // Color
  }

  // Printing Sodium in aqua using ASCII Escape Seqenz to qemu debug output
  i686_io_debug("\033[38;5;6;48;5;0mSODIUM\033[0m\n");

  return;
}
