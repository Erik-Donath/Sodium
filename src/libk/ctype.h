#pragma once

#include <stdbool.h>

bool isalnum(int c);
bool isalpha(int c);
bool isascii(int c);
bool isblank(int c);
bool iscntrl(int c);
bool isdigit(int c);
bool isgraph(int c);
bool islower(int c);
bool isprint(int c);
bool ispunct(int c);
bool isspace(int c);
bool isupper(int c);
bool isxdigit(int c);

int tolower(int c);
int toupper(int c);