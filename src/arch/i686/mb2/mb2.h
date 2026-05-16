#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct i686_mb2_tag {
    uint32_t type;
    uint32_t size;
    uint8_t  data[0];
} __attribute__((packed)) i686_mb2_tag_t;

typedef struct i686_mb2_header {
    uint32_t total_size;
    uint32_t reserved;
    i686_mb2_tag_t tags[0];
} __attribute__((packed)) i686_mb2_header_t;

bool i686_mb2_parse(i686_mb2_header_t* header);
