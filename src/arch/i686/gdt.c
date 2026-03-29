#include "gdt.h"

#include <stdint.h>

/*
See: https://wiki.osdev.org/Global_Descriptor_Table

Global Decriptor Table Pointer:
| Bits    | Description                               |
|---------|-------------------------------------------|
| 0-15    | Table limit                               |
| 16-47   | Pointer to the first Entry                |

Global Decriptor Table Entry:
| Bits    | Description                               |
|---------|-------------------------------------------|
| 0-15    | Segment Limit (lower 16 bits)             |
| 16-31   | Base Address (lower 16 bits)              |
| 32-39   | Base Address (middle 8 bits)              |
| 40-47   | Access Rights                             |
| 48-51   | Segment Limit (upper 4 bits)              |
| 52-55   | Flags                                     |
| 56-63   | Base Address (upper 8 bits)               |
*/

typedef enum i686_gdt_access : uint8_t {
  // 7: P (Present) bit
  GDT_ACCESS_PRESENT = 0x80,

  // 5-6: DPL (Descriptor Privilege Level) bits
  GDT_ACCESS_RING0 = 0x00,
  GDT_ACCESS_RING1 = 0x20,
  GDT_ACCESS_RING2 = 0x40,
  GDT_ACCESS_RING3 = 0x60,

  // 4: S (Descriptor type) bit
  GDT_ACCESS_SYSTEM = 0x00,
  GDT_ACCESS_CODE_DATA = 0x10,

  // 3: E (Executable) bit
  GDT_ACCESS_EXECUTABLE = 0x08,

  // 2: DC (Direction/Conforming) bit
  GDT_ACCESS_DATA_DIRECTION_DOWN = 0x04,
  GDT_ACCESS_CODE_CONFORMING = 0x04,

  // 1: RW (Readable/Writable) bit
  GDT_ACCESS_CODE_READABLE = 0x02,
  GDT_ACCESS_DATA_WRITEABLE = 0x02,

  // 0: A (Accessed) bit
  GDT_ACCESS_ACCESSED = 0x01,
} i686_gdt_access;

typedef enum i686_gdt_flags : uint8_t {
  // 3: G (Granularity flag)
  GDT_FLAG_GRANULARITY_1B = 0x00,
  GDT_FLAG_GRANULARITY_4K = 0x08,

  // 2: DB (Size Flag)
  GDT_FLAG_16BIT = 0x00,
  GDT_FLAG_32BIT = 0x04,

  // 1: L (Long Mode)
  GDT_FLAG_64BIT = 0x2,

  // 0: Reserved
  GDT_FLAG_AVAILABLE = 0x1, // #FIXME: Might be removed in the future
} i686_gdt_flags;

typedef struct __attribute__((packed)) i686_gdt_entry {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_middle;
  uint8_t access;
  uint8_t limit_high : 4;
  uint8_t flags : 4;
  uint8_t base_high;
} i686_gdt_entry;

typedef struct __attribute__((packed)) i686_gdt_pointer {
  uint16_t limit;
  i686_gdt_entry *base;
} i686_gdt_pointer;

#define GDT_ENTRY_COUNT 5
static i686_gdt_entry gdt[GDT_ENTRY_COUNT] = {0};

static i686_gdt_pointer gdt_ptr = {
    .limit = sizeof(gdt) - 1,
    .base = gdt,
};

void i686_gdt_set(uint8_t segnum, uint32_t base, uint32_t limit, uint8_t access,
                  uint8_t flags) {
  gdt[segnum] = (i686_gdt_entry){
      .limit_low = (uint16_t)((limit) & 0xFFFF),
      .base_low = (uint16_t)((base) & 0xFFFF),
      .base_middle = (uint8_t)((base >> 16) & 0xFF),
      .access = (uint8_t)access,
      .limit_high = (uint8_t)((limit >> 16) & 0x0F),
      .flags = (uint8_t)(flags & 0x0F),
      .base_high = (uint8_t)((base >> 24) & 0xFF),
  };
}

// Defined in Assembly
extern void __attribute__((cdecl)) i686_gdt_flush(i686_gdt_pointer *gdt_ptr,
                                                  uint16_t kernel_code_segment,
                                                  uint16_t kernel_data_segment);

void i686_gdt_init(void) {
  // Null Descriptior
  i686_gdt_set(0, 0, 0, 0, 0);

  // Kernel 32-bit Code Segment
  i686_gdt_set(1, 0x00000, 0xFFFFF,
               GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_DATA |
                   GDT_ACCESS_EXECUTABLE | GDT_ACCESS_CODE_READABLE,
               GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K);

  // Kernel 32-bit Data Segment
  i686_gdt_set(2, 0x00000, 0xFFFFF,
               GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_DATA |
                   GDT_ACCESS_DATA_WRITEABLE,
               GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K);

  // User 32-bit Code Segment
  i686_gdt_set(3, 0x00000, 0xFFFFF,
               GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_CODE_DATA |
                   GDT_ACCESS_EXECUTABLE | GDT_ACCESS_CODE_READABLE,
               GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K);

  // User 32-bit Data Segment
  i686_gdt_set(4, 0x00000, 0xFFFFF,
               GDT_ACCESS_PRESENT | GDT_ACCESS_RING3 | GDT_ACCESS_CODE_DATA |
                   GDT_ACCESS_DATA_WRITEABLE,
               GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4K);

  // Flush GDT
  i686_gdt_flush(&gdt_ptr, i686_GDT_KERNEL_CODE_SEGMENT,
                 i686_GDT_KERNEL_DATA_SEGMENT);
}
