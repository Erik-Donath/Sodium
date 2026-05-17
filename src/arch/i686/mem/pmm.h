#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "map.h"

// The mem info struct and map musst be available when calling the function. After the call the memory info is stored.
bool i686_mem_pmm_init(const i686_mem_info_t* info);

void* i686_mem_pmm_alloc(void);
void i686_mem_pmm_free(void* ptr);
