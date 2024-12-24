#pragma once
#include <kernel/util.h>

void C_CALL pre_main(void* mb_info);
extern void ASM_CALL __attribute__((noreturn)) i686_shutdown();
