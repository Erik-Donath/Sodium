#include "pre_kernel.h"
#include "io.h"

static void e9_print(const char *str) {
  while (*str)
    outb(0xE9, (uint8_t)*str++);
}

void __attribute__((cdecl)) pre_kernel(void *mb_info) {
  (void)mb_info;
  e9_print("pre_kernel reached\n");

  for (;;) {
    e9_print("Hi\n");
    e9_print("Ho\n");
  }
  return; // Shutdown via booloader/i686/shutdown.asm
}
