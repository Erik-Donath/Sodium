#pragma once

#include "arch/i686/memory/heap.h"

#define malloc(size) i686_memory_malloc(size)
#define free(block) i686_memory_free(block)
