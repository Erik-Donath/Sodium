#pragma once

#include <stdint.h>

#define i686_GDT_NULL_DESCRIPTOR_SEGMENT 0x00
#define i686_GDT_KERNEL_CODE_SEGMENT 0x08
#define i686_GDT_KERNEL_DATA_SEGMENT 0x10
#define i686_GDT_USER_CODE_SEGMENT 0x18
#define i686_GDT_USER_DATA_SEGMENT 0x20
#define i686_GDT_TSS_SEGMENT 0x28

// See: https://wiki.osdev.org/Segment_Selector

#define RPL0 0x00
#define RPL1 0x01
#define RPL2 0x02
#define RPL3 0x03

#define TI_GDT 0x00
#define TI_LDT 0x04

#define SEGMENT(segment_number, rpl, ti) ((segment_number) | (rpl) | (ti))
