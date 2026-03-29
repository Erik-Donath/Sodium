#include "pre_kernel.h"
#include "io.h"

void __attribute__((cdecl)) pre_kernel(void *mb_info) {
  (void)mb_info;

  // Printing Sodium in aqua using ASCII Escape Seqenz to qemu debug output
  i686_io_debug("\033[38;5;6;48;5;0mSODIUM\033[0m\n");

  return; // Shutdown via booloader/i686/shutdown.asm
}
