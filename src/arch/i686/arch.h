
#pragma once
#include "../../kernel/libc/util.h"
#include "boot/multiboot2/multiboot.h"

// Architecture-specific entry point before kernel main
void C_CALL pre_main(mb_info_ptr mb_info);

// Shutdown routine for i686 architecture
extern void ASM_CALL NORETURN i686_shutdown();
