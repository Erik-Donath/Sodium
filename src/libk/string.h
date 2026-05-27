#pragma once

#include <stddef.h>
#include <stdint.h>

// @brief Copy n bytes from src to dest. Regions must not overlap.
void *memcpy(void *dest, const void *src, size_t n);

// @brief Copy n bytes from src to dest, handling overlapping regions.
void *memmove(void *dest, const void *src, size_t n);

// @brief Fill n bytes of s with byte value c.
void *memset(void *s, int c, size_t n);

// @brief Compare n bytes of s1 and s2. Returns negative / 0 / positive.
int memcmp(const void *s1, const void *s2, size_t n);

// @brief Find first occurrence of byte c in the first n bytes of s.
void *memchr(const void *s, int c, size_t n);

// @brief Return the length of NUL-terminated string s, excluding the NUL.
size_t strlen(const char *s);

// @brief Like strlen(), but scans at most maxlen bytes.
size_t strnlen(const char *s, size_t maxlen);

// @brief Copy NUL-terminated src into dest, including the NUL.
char *strcpy(char *dest, const char *src);

// @brief Copy at most n bytes of src into dest, padding with NUL to fill n.
char *strncpy(char *dest, const char *src, size_t n);

// @brief Append NUL-terminated src to the end of dest.
char *strcat(char *dest, const char *src);

// @brief Append at most n characters of src to dest; always NUL-terminates.
char *strncat(char *dest, const char *src, size_t n);

// @brief Lexicographic comparison of two NUL-terminated strings.
int strcmp(const char *s1, const char *s2);

// @brief Like strcmp(), but compares at most n characters.
int strncmp(const char *s1, const char *s2, size_t n);

// @brief Find first occurrence of c in s (including the NUL terminator).
char *strchr(const char *s, int c);

// @brief Find last occurrence of c in s.
char *strrchr(const char *s, int c);

// @brief Find first occurrence of needle as a substring of haystack.
char *strstr(const char *haystack, const char *needle);

// @brief Find first character of s that also appears in accept.
char *strpbrk(const char *s, const char *accept);

// @brief Length of initial segment of s whose characters are all in accept.
size_t strspn(const char *s, const char *accept);

// @brief Length of initial segment of s with no characters in reject.
size_t strcspn(const char *s, const char *reject);

// @brief Tokenise a string by splitting on delimiter characters.
//        NOTE: uses a static internal pointer: not re-entrant.
//              Pass NULL for s on subsequent calls to continue tokenising.
char *strtok(char *s, const char *delim);
