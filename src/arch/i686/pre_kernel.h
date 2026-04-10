#pragma once

// Called from boot.asm after stack setup and BSS zeroing.
// mb_info is the Multiboot2 info struct pointer passed by GRUB.
// Return ends up in an system shutdown.
void __attribute__((cdecl)) i686_pre_kernel(void* mb_info);
