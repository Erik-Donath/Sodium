#include "string.h"

// Internal

// (memcpy, memset, memcmp, strlen are implemented in arch/i686/libk/string.asm
//  as optimised NASM routines. The C fallbacks here cover the remaining
//  functions that are not performance-critical.)

// Definitions

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t       *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    if (!n || d == s) return dest;
    if (d < s || d >= s + n) return memcpy(dest, src, n);
    d += n; s += n;
    while (n--) *--d = *--s;
    return dest;
}

void *memchr(const void *s, int c, size_t n) {
    const uint8_t *p  = (const uint8_t *)s;
    const uint8_t  ch = (uint8_t)c;
    while (n--) { if (*p == ch) return (void *)p; p++; }
    return NULL;
}

size_t strnlen(const char *s, size_t maxlen) {
    size_t i = 0;
    while (i < maxlen && s[i]) i++;
    return i;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i]; i++) dest[i] = src[i];
    for (; i < n; i++) dest[i] = '\0';
    return dest;
}

char *strcat(char *dest, const char *src) {
    char *d = dest + strlen(dest);
    while ((*d++ = *src++));
    return dest;
}

char *strncat(char *dest, const char *src, size_t n) {
    char *d = dest + strlen(dest);
    while (n-- && *src) *d++ = *src++;
    *d = '\0';
    return dest;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s1 == *s2) { s1++; s2++; }
    return (int)(unsigned char)*s1 - (int)(unsigned char)*s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    for (; n; n--) {
        unsigned char c1 = (unsigned char)*s1++;
        unsigned char c2 = (unsigned char)*s2++;
        if (c1 != c2) return (int)c1 - (int)c2;
        if (!c1)      return 0;
    }
    return 0;
}

char *strchr(const char *s, int c) {
    char ch = (char)c;
    while (*s) { if (*s == ch) return (char *)s; s++; }
    return (ch == '\0') ? (char *)s : NULL;
}

char *strrchr(const char *s, int c) {
    char        ch   = (char)c;
    const char *last = NULL;
    do { if (*s == ch) last = s; } while (*s++);
    return (char *)last;
}

char *strstr(const char *haystack, const char *needle) {
    if (!*needle) return (char *)haystack;
    for (; *haystack; haystack++) {
        const char *h = haystack, *n = needle;
        while (*h && *n && *h == *n) { h++; n++; }
        if (!*n) return (char *)haystack;
    }
    return NULL;
}

size_t strspn(const char *s, const char *accept) {
    size_t n = 0;
    while (*s && strchr(accept, *s)) { n++; s++; }
    return n;
}

size_t strcspn(const char *s, const char *reject) {
    size_t n = 0;
    while (*s && !strchr(reject, *s)) { n++; s++; }
    return n;
}

char *strpbrk(const char *s, const char *accept) {
    s += strcspn(s, accept);
    return *s ? (char *)s : NULL;
}

char *strtok(char *s, const char *delim) {
    static char *saved = NULL;
    if (s) saved = s;
    if (!saved || !*saved) return NULL;
    saved += strspn(saved, delim);
    if (!*saved) { saved = NULL; return NULL; }
    char *token = saved;
    saved += strcspn(saved, delim);
    if (*saved) *saved++ = '\0';
    else        saved = NULL;
    return token;
}
