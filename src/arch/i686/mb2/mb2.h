#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "../mem/map.h"

typedef struct i686_mb2_tag {
    uint32_t type;
    uint32_t size;
    uint8_t  data[0];
} __attribute__((packed)) i686_mb2_tag_t;

typedef struct i686_mb2_header {
    uint32_t       total_size;
    uint32_t       reserved;
    i686_mb2_tag_t tags[0];
} __attribute__((packed)) i686_mb2_header_t;

// @brief Parse the Multiboot2 information structure provided by GRUB.
//        Copies the memory map and basic memory info into an internal buffer.
//        WARNING: The MB2 struct lives in unprotected memory and may be
//        overwritten after i686_mem_pmm_init() completes.
// @param header Physical pointer to the MB2 fixed part (EBX from GRUB).
// @return true on success; false if a required tag is absent or an invalid structure was given.
bool i686_mb2_parse(i686_mb2_header_t *header);

// @brief Return the memory info gathered by the last successful parse.
//        WARNING: Call after parsing the MB2 with i686_mb2_parse.
// @return Pointer to the internal buffer.
const i686_mem_info_t *i686_mb2_get_mem_info(void);
