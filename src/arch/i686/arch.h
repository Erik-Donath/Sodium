#pragma once
#include "kernel/libc/util.h"
#include "boot/multiboot2/multiboot.h"

void C_CALL pre_main(mb_info_ptr mb_info);
extern void ASM_CALL NORETURN i686_shutdown();
