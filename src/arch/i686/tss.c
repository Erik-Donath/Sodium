#include "tss.h"

#include <stdint.h>

typedef struct __attribute__((packed)) i686_tss_table {
  uint32_t cool;
} i686_tss_table;

i686_tss_table tss = {0};

void i686_tss_init(void) { return; }
void i686_tss_load(void) { return; }