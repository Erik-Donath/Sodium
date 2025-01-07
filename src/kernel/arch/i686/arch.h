#pragma once
#include <kernel/util.h>
#include "mb2/multiboot.h"

#define NORETURN __attribute__((noreturn))

void NORETURN panic(const char* msg);

void C_CALL pre_main(mb_info_ptr mb_info);
extern void ASM_CALL NORETURN i686_shutdown();
