#pragma once
#include <kernel/util.h>
#include "mb2/multiboot.h"

void C_CALL pre_main(mb_info_ptr mb_info);
extern void ASM_CALL __attribute__((noreturn)) i686_shutdown();
