#include "ctype.h"

#include <stdint.h>

typedef enum ctype_type : uint8_t {
  TYPE_NONE   = 0x00,
  TYPE_DIGIT  = 0x01,
  TYPE_UPPER  = 0x02,
  TYPE_LOWER  = 0x04,
  TYPE_XDIGIT = 0x08,
  TYPE_CNTRL  = 0x10,
  TYPE_SPACE  = 0x20,
  TYPE_PUNCT  = 0x40,
  TYPE_GRAPH  = 0x80,
} ctype_type_t;

// Lookup table for ASCII character classifications (0-255).
_Static_assert(sizeof(char) == sizeof(uint8_t), "char must be same size as uint8_t!");
static const ctype_type_t ctype_table[256] = {
  [0 ... 255] = TYPE_NONE,

  // Control characters: 0x00-0x1F and 0x7F.
  [0x00 ... 0x1F] = TYPE_CNTRL,
  [0x7F] = TYPE_CNTRL,

  // Whitespace characters
  [' ']  = TYPE_SPACE,                    // Space
  ['\t'] = TYPE_SPACE | TYPE_CNTRL,       // Horizontal tab
  ['\n'] = TYPE_SPACE | TYPE_CNTRL,       // Line feed
  ['\v'] = TYPE_SPACE | TYPE_CNTRL,       // Vertical tab
  ['\f'] = TYPE_SPACE | TYPE_CNTRL,       // Form feed
  ['\r'] = TYPE_SPACE | TYPE_CNTRL,       // Carriage return

  // Digits 0-9.
  ['0' ... '9'] = TYPE_DIGIT | TYPE_XDIGIT | TYPE_GRAPH,

  // Uppercase letters A-Z (A-F also TYPE_XDIGIT)
  ['A'] = TYPE_UPPER | TYPE_XDIGIT | TYPE_GRAPH,
  ['B'] = TYPE_UPPER | TYPE_XDIGIT | TYPE_GRAPH,
  ['C'] = TYPE_UPPER | TYPE_XDIGIT | TYPE_GRAPH,
  ['D'] = TYPE_UPPER | TYPE_XDIGIT | TYPE_GRAPH,
  ['E'] = TYPE_UPPER | TYPE_XDIGIT | TYPE_GRAPH,
  ['F'] = TYPE_UPPER | TYPE_XDIGIT | TYPE_GRAPH,
  ['G' ... 'Z'] = TYPE_UPPER | TYPE_GRAPH,

  // Lowercase letters a-z (a-f also TYPE_XDIGIT)
  ['a'] = TYPE_LOWER | TYPE_XDIGIT | TYPE_GRAPH,
  ['b'] = TYPE_LOWER | TYPE_XDIGIT | TYPE_GRAPH,
  ['c'] = TYPE_LOWER | TYPE_XDIGIT | TYPE_GRAPH,
  ['d'] = TYPE_LOWER | TYPE_XDIGIT | TYPE_GRAPH,
  ['e'] = TYPE_LOWER | TYPE_XDIGIT | TYPE_GRAPH,
  ['f'] = TYPE_LOWER | TYPE_XDIGIT | TYPE_GRAPH,
  ['g' ... 'z'] = TYPE_LOWER | TYPE_GRAPH,

  // Punctuation and graphical
  [0x21 ... 0x2F] = TYPE_GRAPH | TYPE_PUNCT,  // ! " # $ % & ' ( ) * + , - . /
  [0x3A ... 0x40] = TYPE_GRAPH | TYPE_PUNCT,  // : ; < = > ? @
  [0x5B ... 0x60] = TYPE_GRAPH | TYPE_PUNCT,  // [ \ ] ^ _ `
  [0x7B ... 0x7E] = TYPE_GRAPH | TYPE_PUNCT,  // { | } ~
};

#define is(c, type) ((ctype_table[(uint8_t)(c)] & (type)) != 0)

bool isalnum(char c)  { return is(c, TYPE_DIGIT | TYPE_UPPER | TYPE_LOWER); }
bool isalpha(char c)  { return is(c, TYPE_UPPER | TYPE_LOWER); }
bool isascii(char c)  { return ((uint8_t)c) <= 0x7F; }
bool isblank(char c)  { return c == ' ' || c == '\t'; }
bool iscntrl(char c)  { return is(c, TYPE_CNTRL); }
bool isdigit(char c)  { return is(c, TYPE_DIGIT); }
bool isgraph(char c)  { return is(c, TYPE_GRAPH); }
bool islower(char c)  { return is(c, TYPE_LOWER); }
bool isprint(char c)  { return is(c, TYPE_GRAPH) || c == ' '; }
bool ispunct(char c)  { return is(c, TYPE_PUNCT); }
bool isspace(char c)  { return is(c, TYPE_SPACE); }
bool isupper(char c)  { return is(c, TYPE_UPPER); }
bool isxdigit(char c) { return is(c, TYPE_XDIGIT); }

// This code might contain bugs!