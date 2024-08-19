#pragma once

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define ARRAY_FIRST(x) (x[0])
#define ARRAY_LAST(x) (x[ARRAY_SIZE(x) - 1])
