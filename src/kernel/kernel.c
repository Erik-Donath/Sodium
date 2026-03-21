#include "kernel.h"
#include <kernel/libc/stdio.h>

void kmain() {
  printf("Sodium Loaded\n\n");
  terminal_testColor();
  puts("\n\033[0m" Color(TERMINAL_COLOR_WHITE, TERMINAL_COLOR_BLACK) "> ");
}
