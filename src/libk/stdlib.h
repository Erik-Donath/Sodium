#pragma once

#include <stddef.h>
#include <stdint.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

// @brief Allocate size bytes from the kernel heap. Returns NULL on failure.
void *malloc(size_t size);

// @brief Return a block to the kernel heap. NULL is a no-op.
void free(void *ptr);

// @brief Allocate nmemb*size zero-initialised bytes.
//        Returns NULL on overflow or OOM.
void *calloc(size_t nmemb, size_t size);

// @brief Resize an allocation.
//        calloc(NULL, n) == malloc(n).
//        realloc(ptr, 0) == free(ptr), returns NULL.
//        Returns NULL without freeing ptr if the new allocation fails.
void *realloc(void *ptr, size_t new_size);

// @brief Parse a long in the given base (0 = auto-detect 0x / 0 / decimal).
long strtol(const char *str, char **endptr, int base);

// @brief Parse an unsigned long in the given base.
unsigned long strtoul(const char *str, char **endptr, int base);

// @brief Parse a long long in the given base.
long long strtoll(const char *str, char **endptr, int base);

// @brief Parse an unsigned long long in the given base.
unsigned long long strtoull(const char *str, char **endptr, int base);

// @brief Equivalent to (int)strtol(str, NULL, 10).
int atoi(const char *str);

// @brief Equivalent to strtol(str, NULL, 10).
long atol(const char *str);

// @brief Equivalent to strtoll(str, NULL, 10).
long long atoll(const char *str);

// @brief Absolute value. Behaviour is undefined if x == *_MIN.
int       abs  (int       x);
long      labs (long      x);
long long llabs(long long x);
