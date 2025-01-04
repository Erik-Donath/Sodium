#pragma once
#include <stdbool.h>

typedef const void* mb_info_ptr;

bool mb_parse(mb_info_ptr mb);
void mb_print(mb_info_ptr mb);