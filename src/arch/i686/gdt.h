#pragma once

#define i686_GDT_NULL_DESCRIPTOR_SEGMENT 0x00
#define i686_GDT_KERNEL_CODE_SEGMENT 0x08
#define i686_GDT_KERNEL_DATA_SEGMENT 0x10
#define i686_GDT_USER_CODE_SEGMENT 0x18
#define i686_GDT_USER_DATA_SEGMENT 0x20
#define i686_GDT_TSS_SEGMENT 0x28

void i686_gdt_init(void);
void i686_gdt_load(void);