#pragma once

enum {
    GDT_KERNEL_CODE_SEGMENT = 0x08,
    GDT_KERNEL_DATA_SEGMENT = 0x10
};

void i686_GDT_Initialize();
