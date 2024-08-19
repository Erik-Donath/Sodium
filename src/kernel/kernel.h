#pragma once

void kernel_welcome();
void printf_test();
extern void __attribute__((cdecl)) kernel_main(void* mb_info_struct);
