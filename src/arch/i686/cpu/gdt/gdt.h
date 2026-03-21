#pragma once

enum {
    GDT_NULL_DESCRIPTOR_SEGMENT = 0x00,
    GDT_KERNEL_CODE_SEGMENT     = 0x08,
    GDT_KERNEL_DATA_SEGMENT     = 0x10,
    GDT_USER_CODE_SEGMENT       = 0x18,
    GDT_USER_DATA_SEGMENT       = 0x20,
    GDT_TSS_SEGMENT             = 0x28
};

void i686_GDT_Initialize();
