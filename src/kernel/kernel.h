#pragma once
#include "util/assembly.h"
#include "sysinfo.h"

void kernel_welcome();
extern void ASMCALL kernel_main(void* mb_info_struct);
