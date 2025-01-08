#pragma once

#include "arch/i686/memory/heap.h"

#define malloc(size) i686_memory_malloc((size))
#define free(ptr) i686_memory_free((ptr))
