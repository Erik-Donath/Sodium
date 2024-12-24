#pragma once

// GDT segment selectors
#define GDT_KERNEL_CODE_SEGMENT 0x08
#define GDT_KERNEL_DATA_SEGMENT 0x10

void i686_GDT_Initialize();
