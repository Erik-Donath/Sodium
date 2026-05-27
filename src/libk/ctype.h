#pragma once

#include <stdbool.h>

// @brief True if c is alphanumeric (A-Z, a-z, 0-9).
bool isalnum(int c);

// @brief True if c is alphabetic (A-Z, a-z).
bool isalpha(int c);

// @brief True if c fits in a 7-bit ASCII value (0-127).
bool isascii(int c);

// @brief True if c is a horizontal tab or space.
bool isblank(int c);

// @brief True if c is an ASCII control character (0x00-0x1F, 0x7F).
bool iscntrl(int c);

// @brief True if c is a decimal digit (0-9).
bool isdigit(int c);

// @brief True if c is a printable, non-space character.
bool isgraph(int c);

// @brief True if c is a lowercase letter (a-z).
bool islower(int c);

// @brief True if c is a printable character (including space).
bool isprint(int c);

// @brief True if c is a punctuation character.
bool ispunct(int c);

// @brief True if c is whitespace (space, tab, LF, VT, FF, CR).
bool isspace(int c);

// @brief True if c is an uppercase letter (A-Z).
bool isupper(int c);

// @brief True if c is a hexadecimal digit (0-9, A-F, a-f).
bool isxdigit(int c);

// @brief Convert c to lowercase if uppercase; otherwise return c unchanged.
int tolower(int c);

// @brief Convert c to uppercase if lowercase; otherwise return c unchanged.
int toupper(int c);
