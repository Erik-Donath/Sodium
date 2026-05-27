#include "stdlib.h"
#include "ctype.h"
#include "string.h"

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#include "../kernel/memory/heap.h"

// #FIXME: This File needs revision!

// Internal

// @brief Parse unsigned integer digits from p[] in the given base.
//        Sets *end to one past the last digit consumed, or NULL if none were.
//        On overflow, consumes all remaining valid digits and returns ULLONG_MAX.
static unsigned long long stdlib_parse_uint(const char *p, unsigned base,
                                             const char **end);

// @brief Strip leading whitespace, consume optional sign, resolve base prefix.
//        Advances *p past everything consumed.
// @return true if a '-' sign was found.
static bool stdlib_strto_prefix(const char **p, int *base);

// Definitions

static unsigned long long stdlib_parse_uint(const char *p, unsigned base,
                                             const char **end) {
    unsigned long long val = 0;
    bool               any = false;

    while (*p) {
        unsigned char c = (unsigned char)*p;
        unsigned      d;
        if      (c >= '0' && c <= '9') d = (unsigned)(c - '0');
        else if (c >= 'a' && c <= 'z') d = (unsigned)(c - 'a') + 10u;
        else if (c >= 'A' && c <= 'Z') d = (unsigned)(c - 'A') + 10u;
        else break;
        if (d >= base) break;

        if (val > ((unsigned long long)-1 - d) / base) {
            do {
                p++;
                c = (unsigned char)*p;
                if      (c >= '0' && c <= '9') d = (unsigned)(c - '0');
                else if (c >= 'a' && c <= 'z') d = (unsigned)(c - 'a') + 10u;
                else if (c >= 'A' && c <= 'Z') d = (unsigned)(c - 'A') + 10u;
                else break;
            } while (d < base);
            *end = p;
            return (unsigned long long)-1;
        }

        val = val * base + d;
        any = true;
        p++;
    }

    *end = any ? p : NULL;
    return val;
}

static bool stdlib_strto_prefix(const char **p, int *base) {
    while (isspace((unsigned char)**p)) (*p)++;

    bool neg = (**p == '-');
    if (**p == '-' || **p == '+') (*p)++;

    if ((*base == 0 || *base == 16) &&
        (*p)[0] == '0' && ((*p)[1] == 'x' || (*p)[1] == 'X')) {
        *base  = 16;
        *p    += 2;
    } else if (*base == 0) {
        *base = ((*p)[0] == '0') ? 8 : 10;
    }
    return neg;
}

void *malloc(size_t size)  { return k_heap_alloc(size); }
void  free  (void *ptr)    { k_heap_free(ptr); }

void *calloc(size_t nmemb, size_t size) {
    if (nmemb && size > (size_t)-1 / nmemb) return NULL;
    size_t total = nmemb * size;
    void *p = k_heap_alloc(total);
    if (p) memset(p, 0, total);
    return p;
}

void *realloc(void *ptr, size_t new_size) {
    if (!ptr)      return k_heap_alloc(new_size);
    if (!new_size) { k_heap_free(ptr); return NULL; }
    void *np = k_heap_alloc(new_size);
    if (!np) return NULL;
    size_t old = k_heap_alloc_size(ptr);
    memcpy(np, ptr, old < new_size ? old : new_size);
    k_heap_free(ptr);
    return np;
}

unsigned long long strtoull(const char *str, char **endptr, int base) {
    const char *p   = str;
    bool        neg = stdlib_strto_prefix(&p, &base);
    const char *end;
    unsigned long long val = stdlib_parse_uint(p, (unsigned)base, &end);
    if (!end) { if (endptr) *endptr = (char *)str; return 0; }
    if (endptr) *endptr = (char *)end;
    return neg ? (0ULL - val) : val;
}

long long strtoll(const char *str, char **endptr, int base) {
    const char *p   = str;
    bool        neg = stdlib_strto_prefix(&p, &base);
    const char *end;
    unsigned long long uval = stdlib_parse_uint(p, (unsigned)base, &end);
    if (!end) { if (endptr) *endptr = (char *)str; return 0; }
    if (endptr) *endptr = (char *)end;
    if (neg) {
        if (uval > (unsigned long long)LLONG_MAX + 1ULL) return LLONG_MIN;
        return (long long)(0ULL - uval);
    }
    if (uval > (unsigned long long)LLONG_MAX) return LLONG_MAX;
    return (long long)uval;
}

unsigned long strtoul(const char *str, char **endptr, int base) {
    unsigned long long v = strtoull(str, endptr, base);
    return (v > (unsigned long)-1) ? (unsigned long)-1 : (unsigned long)v;
}

long strtol(const char *str, char **endptr, int base) {
    long long v = strtoll(str, endptr, base);
    if (v > LONG_MAX) return LONG_MAX;
    if (v < LONG_MIN) return LONG_MIN;
    return (long)v;
}

int       atoi (const char *str) { return (int) strtol (str, NULL, 10); }
long      atol (const char *str) { return       strtol (str, NULL, 10); }
long long atoll(const char *str) { return       strtoll(str, NULL, 10); }

int       abs  (int       x) { return x < 0 ? -x : x; }
long      labs (long      x) { return x < 0 ? -x : x; }
long long llabs(long long x) { return x < 0 ? -x : x; }
