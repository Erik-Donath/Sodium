#pragma once
#include <stdbool.h>
#include <stddef.h>

extern char os_start;
extern char os_end;

typedef enum MEM_ERROR {
    MEM_ERROR_OK = 0,
    MEM_ERROR_NO_BLOCK_FOUND = 1,
} MEM_ERROR_t;

MEM_ERROR_t i686_memory_init();
void i686_memory_info();
void* i686_memory_malloc(size_t size);
void i686_memory_free(void* block);
void i686_memory_coalesce_adjacent_blocks();
