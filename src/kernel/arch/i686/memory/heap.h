#pragma once
#include <stdbool.h>
#include <stddef.h>

extern char os_start;
extern char os_end;

bool i686_memory_init();
void i686_memory_info();
void* i686_memory_malloc(size_t size);
void i686_memory_free(void* block);
