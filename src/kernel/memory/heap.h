#pragma once
#include <stddef.h>
#include <stdint.h>

// The heap might be initilised before k_main. The second init call will be ignored!

// It is expected that pmm is allready initialised
void  k_heap_init();
// size can not be bigger than page size (Normally 4096)
void* k_heap_alloc(size_t size);
void  k_heap_free(void* ptr);
