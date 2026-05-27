#include "ctype.h"

#include <stdint.h>

// Internal

_Static_assert(sizeof(char) == sizeof(uint8_t),
               "char and uint8_t must be the same size.");

enum ctype_flag : uint8_t {
    TYPE_NONE   = 0x00,
    TYPE_DIGIT  = 0x01,
    TYPE_UPPER  = 0x02,
    TYPE_LOWER  = 0x04,
    TYPE_XDIGIT = 0x08,
    TYPE_CNTRL  = 0x10,
    TYPE_SPACE  = 0x20,
    TYPE_PUNCT  = 0x40,
    TYPE_GRAPH  = 0x80,
};

static const uint8_t ctype_table[128] = {
    // 0x00-0x08: control
    [0x00] = TYPE_CNTRL, [0x01] = TYPE_CNTRL, [0x02] = TYPE_CNTRL,
    [0x03] = TYPE_CNTRL, [0x04] = TYPE_CNTRL, [0x05] = TYPE_CNTRL,
    [0x06] = TYPE_CNTRL, [0x07] = TYPE_CNTRL, [0x08] = TYPE_CNTRL,
    // 0x09-0x0D: whitespace control
    [0x09] = TYPE_CNTRL | TYPE_SPACE,  // HT
    [0x0A] = TYPE_CNTRL | TYPE_SPACE,  // LF
    [0x0B] = TYPE_CNTRL | TYPE_SPACE,  // VT
    [0x0C] = TYPE_CNTRL | TYPE_SPACE,  // FF
    [0x0D] = TYPE_CNTRL | TYPE_SPACE,  // CR
    // 0x0E-0x1F: control
    [0x0E] = TYPE_CNTRL, [0x0F] = TYPE_CNTRL, [0x10] = TYPE_CNTRL,
    [0x11] = TYPE_CNTRL, [0x12] = TYPE_CNTRL, [0x13] = TYPE_CNTRL,
    [0x14] = TYPE_CNTRL, [0x15] = TYPE_CNTRL, [0x16] = TYPE_CNTRL,
    [0x17] = TYPE_CNTRL, [0x18] = TYPE_CNTRL, [0x19] = TYPE_CNTRL,
    [0x1A] = TYPE_CNTRL, [0x1B] = TYPE_CNTRL, [0x1C] = TYPE_CNTRL,
    [0x1D] = TYPE_CNTRL, [0x1E] = TYPE_CNTRL, [0x1F] = TYPE_CNTRL,
    // 0x20: space
    [0x20] = TYPE_SPACE,
    // 0x21-0x2F: punctuation
    [0x21 ... 0x2F] = TYPE_GRAPH | TYPE_PUNCT,
    // 0x30-0x39: digits
    [0x30 ... 0x39] = TYPE_DIGIT | TYPE_XDIGIT | TYPE_GRAPH,
    // 0x3A-0x40: punctuation
    [0x3A ... 0x40] = TYPE_GRAPH | TYPE_PUNCT,
    // 0x41-0x46: uppercase hex (A-F)
    [0x41 ... 0x46] = TYPE_UPPER | TYPE_XDIGIT | TYPE_GRAPH,
    // 0x47-0x5A: uppercase (G-Z)
    [0x47 ... 0x5A] = TYPE_UPPER | TYPE_GRAPH,
    // 0x5B-0x60: punctuation
    [0x5B ... 0x60] = TYPE_GRAPH | TYPE_PUNCT,
    // 0x61-0x66: lowercase hex (a-f)
    [0x61 ... 0x66] = TYPE_LOWER | TYPE_XDIGIT | TYPE_GRAPH,
    // 0x67-0x7A: lowercase (g-z)
    [0x67 ... 0x7A] = TYPE_LOWER | TYPE_GRAPH,
    // 0x7B-0x7E: punctuation
    [0x7B ... 0x7E] = TYPE_GRAPH | TYPE_PUNCT,
    // 0x7F: DEL
    [0x7F] = TYPE_CNTRL,
};

#define ctype_is(c, flag) \
    ((uint8_t)(c) < 128 && (ctype_table[(uint8_t)(c)] & (flag)) != 0)

// Definitions

bool isalnum(int c)  { return ctype_is(c, TYPE_DIGIT | TYPE_UPPER | TYPE_LOWER); }
bool isalpha(int c)  { return ctype_is(c, TYPE_UPPER | TYPE_LOWER); }
bool isascii(int c)  { return (uint8_t)(c) <= 0x7F; }
bool isblank(int c)  { return (c == ' ' || c == '\t') && (uint8_t)(c) <= 0x7F; }
bool iscntrl(int c)  { return ctype_is(c, TYPE_CNTRL); }
bool isdigit(int c)  { return ctype_is(c, TYPE_DIGIT); }
bool isgraph(int c)  { return ctype_is(c, TYPE_GRAPH); }
bool islower(int c)  { return ctype_is(c, TYPE_LOWER); }
bool isprint(int c)  { return ctype_is(c, TYPE_GRAPH) || c == ' '; }
bool ispunct(int c)  { return ctype_is(c, TYPE_PUNCT); }
bool isspace(int c)  { return ctype_is(c, TYPE_SPACE); }
bool isupper(int c)  { return ctype_is(c, TYPE_UPPER); }
bool isxdigit(int c) { return ctype_is(c, TYPE_XDIGIT); }

int tolower(int c) { return isupper(c) ? c + ('a' - 'A') : c; }
int toupper(int c) { return islower(c) ? c - ('a' - 'A') : c; }
