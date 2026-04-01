#include <string.h>

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t*)dest;
    const uint8_t *s = (const uint8_t*)src;
 
    if (n == 0 || d == s)
        return dest;
    if (d < s || d >= s + n) {
        return memcpy(dest, src, n);
    }
 
    d += n;
    s += n;
    while (n--)
        *--d = *--s;
 
    return dest;
}

void *memchr(const void *s, int c, size_t n) {
    const uint8_t *p = (const uint8_t*)s;
    const uint8_t  val = (uint8_t)c;
 
    while (n--) {
        if (*p == val)
            return (void *)p;
        p++;
    }
    return (void *)0;
}

size_t strnlen(const char *s, size_t maxlen) {
    size_t i = 0;
    while (i < maxlen && s[i] != '\0')
        i++;
    return i;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++) != '\0');
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];

    for (; i < n; i++)
        dest[i] = '\0';
    return dest;
}

char *strcat(char *dest, const char *src) {
    char *d = dest + strlen(dest);
    while ((*d++ = *src++) != '\0');
    return dest;
}

char *strncat(char *dest, const char *src, size_t n) {
    char *d = dest + strlen(dest);
    while (n-- && *src != '\0')
        *d++ = *src++;
    *d = '\0';
    return dest;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (int)(unsigned char)*s1 - (int)(unsigned char)*s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    if (n == 0)
        return 0;
    while (n-- && *s1 != '\0' && *s1 == *s2) {
        s1++;
        s2++;
    }
    
    if (n == (size_t)-1)
        return 0;
    return (int)(unsigned char)*s1 - (int)(unsigned char)*s2;
}

char *strchr(const char *s, int c) {
    const char ch = (char)c;
    while (*s != '\0') {
        if (*s == ch)
            return (char *)s;
        s++;
    }
    
    return (ch == '\0') ? (char *)s : (char *)0;
}

char *strrchr(const char *s, int c) {
    const char  ch   = (char)c;
    const char *last = (char *)0;
 
    do {
        if (*s == ch)
            last = s;
    } while (*s++ != '\0');
 
    return (char *)last;
}

char *strstr(const char *haystack, const char *needle) {
    if (*needle == '\0')
        return (char *)haystack;
 
    for (; *haystack != '\0'; haystack++) {
        const char *h = haystack;
        const char *n = needle;
 
        while (*h != '\0' && *n != '\0' && *h == *n) {
            h++;
            n++;
        }
 
        if (*n == '\0')
            return (char *)haystack;
    }
    return (char *)0;
}

size_t strspn(const char *s, const char *accept) {
    size_t n = 0;
    while (*s != '\0' && strchr(accept, *s) != (char *)0) {
        n++;
        s++;
    }
    return n;
}

size_t strcspn(const char *s, const char *reject) {
    size_t n = 0;
    while (*s != '\0' && strchr(reject, *s) == (char *)0) {
        n++;
        s++;
    }
    return n;
}

char *strpbrk(const char *s, const char *accept) {
    s += strcspn(s, accept);
    return (*s != '\0') ? (char *)s : (char *)0;
}

char *strtok(char *s, const char *delim) {
    static char *saved = (char *)0;
 
    if (s != (char *)0)
        saved = s;
 
    if (saved == (char *)0 || *saved == '\0')
        return (char *)0;
 
    saved += strspn(saved, delim);
    if (*saved == '\0') {
        saved = (char *)0;
        return (char *)0;
    }
 

    char *token = saved;
    saved += strcspn(saved, delim);
 
    if (*saved != '\0') {
        *saved++ = '\0';
    } else {
        saved = (char *)0;
    }
 
    return token;
}