#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct printf_sink {
  void (*putc_fn)(char c, void *ctx);
  void *ctx;
} printf_sink_t;

typedef enum exec_flags : uint8_t {
  FLAGS_NONE = 0x00,      // Not given
  FLAGS_ALIGIN = 0x01,    // -
  FLAGS_SIGN = 0x02,      // +
  FLAGS_SPACE = 0x04,     // space
  FLAGS_PAD = 0x08,       // 0
  FLAGS_ALTERNATE = 0x10, // #
} exec_flags_t;

typedef enum number_type : uint8_t {
  NUMBER_DECIMAL_POSITIV = 0,  // %d
  NUMBER_DECIMAL_NEGATIV = 1,  // %d
  NUMBER_DECIMAL_UNSIGNED = 2, // %u
  NUMBER_OCTAL = 3,            // %o
  NUMBER_HEX = 4,              // %x / %X
} number_type_t;

static uint8_t number_type_radix[5] = { 10, 10, 10, 8, 16, };
static char digits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

// Returns how many chars have been written
// prec = -1 => No precision
// width = 0 => No width
uint32_t _print_number(uint64_t value, number_type_t type, bool uppercase, exec_flags_t flags, uint32_t width, int32_t precision, printf_sink_t *sink) {
  char strnum[64];
  uint8_t strnum_count = 0;
  uint8_t radix = number_type_radix[type];

  if(precision == 0 && value == 0) {
    strnum_count = 0;
  }
  else if (value == 0) {
    strnum[strnum_count++] = '0';
  }
  else {
    uint64_t v = value;
    while(v) {
      char digit = digits[v % radix];
      strnum[strnum_count++] = (uppercase && digit >= 'a') ? (char)toupper(digit) : digit;
      v /= radix;
    }
  }

  const char* prefix = NULL;
  if(type == NUMBER_DECIMAL_POSITIV && flags & FLAGS_SIGN) prefix = "+";
  else if(type == NUMBER_DECIMAL_POSITIV && flags & FLAGS_SPACE) prefix = " ";
  else if(type == NUMBER_DECIMAL_NEGATIV) prefix = "-";
  else if(type == NUMBER_OCTAL && flags & FLAGS_ALTERNATE) prefix = "0";
  else if(type == NUMBER_HEX && flags & FLAGS_ALTERNATE && !uppercase) prefix = "0x";
  else if(type == NUMBER_HEX && flags & FLAGS_ALTERNATE && uppercase)  prefix = "0X";

  uint32_t number_zeros = (precision > 0 && precision > strnum_count) ? (precision - strnum_count) : 0;
  uint32_t content_length = (prefix ? strlen(prefix) : 0) + strnum_count + number_zeros;
  uint32_t number_spaces = (width > content_length) ? (width - content_length) : 0;

  bool left_align = flags & FLAGS_ALIGIN;
  char pad_char = (number_zeros == 0 && prefix == NULL && !left_align && flags & FLAGS_PAD) ? '0' : ' ';

  if(!left_align) {
    for(uint32_t i = 0; i < number_spaces; i++) {
      sink->putc_fn(pad_char, sink->ctx);
    }
  }

  if(prefix) {
    for(uint32_t i = 0; prefix[i]; i++) {
      sink->putc_fn(prefix[i], sink->ctx);
    }
  }

  for(uint32_t i = 0; i < number_zeros; i++) {
    sink->putc_fn('0', sink->ctx);
  }

  for(int8_t i = strnum_count - 1; i >= 0; i--) {
    sink->putc_fn(strnum[i], sink->ctx);
  }

  if(left_align) {
    for(uint32_t i = 0; i < number_spaces; i++) {
      sink->putc_fn(' ', sink->ctx);
    }
  }

  return content_length + number_spaces;
}

// Expects fmt to be at the char behind %.
// Returns fmt after the last char of the expression.
const char *_vprintf_exec(const char *fmt, va_list args, printf_sink_t *sink, uint32_t *written) {
  typedef enum exec_lenght : uint8_t {
    LENGHT_NONE = 0,        // Not given
    LENGHT_SHORT = 1,       // h
    LENGHT_SIGNED_CHAR = 2, // hh
    LENGHT_LONG = 3,        // l
    LENGHT_LONGLONG = 4,    // ll
    LENGHT_LONGDOUBLE = 5,  // L
    LENGHT_SIZE = 6,        // z
    LENGHT_INTMAX = 7,      // j
    LENGHT_PTRDIFF = 8,     // t
  } exec_lenght_t;

  exec_flags_t flags = FLAGS_NONE;
  exec_lenght_t lenght = LENGHT_NONE;
  uint32_t width = 0;
  int32_t prec = -1;

  // Flag State
  {
    bool in_flag = true;
    while (in_flag) {
      switch (*fmt) {
      case '-':
        flags |= FLAGS_ALIGIN;
        fmt++;
        break;
      case '+':
        flags |= FLAGS_SIGN;
        fmt++;
        break;
      case ' ':
        flags |= FLAGS_SPACE;
        fmt++;
        break;
      case '0':
        flags |= FLAGS_PAD;
        fmt++;
        break;
      case '#':
        flags |= FLAGS_ALTERNATE;
        fmt++;
        break;
      default:
        in_flag = false;
        break;
      }
    }
  }

  // Width State
  while (*fmt && isdigit(*fmt)) {
    width = 10 * width + (*fmt - '0');
    fmt++;
  }

  // Precision State
  if(*fmt == '.') {
    prec = 0;
    fmt++;

    while (*fmt && isdigit(*fmt)) {
      prec = 10 * prec + (*fmt - '0');
      fmt++;
    }
  }

  // Lenght State
  switch(*fmt) {
    case 'h':
      if(*(fmt + 1) != 'h') {
        lenght = LENGHT_SHORT;
        fmt += 1;
      }
      else {
        lenght = LENGHT_SIGNED_CHAR;
        fmt += 2;
        }
      break;
    case 'l':
      if(*(fmt + 1) != 'l') {
        lenght = LENGHT_LONG;
        fmt += 1;
      }
      else {
        lenght = LENGHT_LONGLONG;
        fmt += 2;
      }
      break;
    case 'L':
      lenght = LENGHT_LONGDOUBLE;
      fmt++;
      break;
    case 'z':
      lenght = LENGHT_SIZE;
      fmt++;
      break;
    case 'j':
      lenght = LENGHT_INTMAX;
      fmt++;
      break;
    case 't':
      lenght = LENGHT_PTRDIFF;
      fmt++;
      break;
  }

  // Specifier Stage
  if(*fmt == '%') {
    sink->putc_fn('%', sink->ctx);
    (*written)++;
    return fmt + 1;
  }

  if(*fmt == 'd' || *fmt == 'i') {
    _print_number(10, NUMBER_DECIMAL_POSITIV, false, flags, width, prec, sink);
  }

  (void)args;
  (void)lenght;

  return fmt + 1;
}

uint32_t _vprintf_core(const char *fmt, va_list args, printf_sink_t *sink) {
  uint32_t written = 0;

  while (*fmt) {
    if (*fmt == '%') {
      fmt = _vprintf_exec((fmt + 1), args, sink, &written);
    } else {
      sink->putc_fn(*(fmt++), sink->ctx);
      written++;
    }
  }

  return written;
}

// #FIXME: Temporarly defining putc to output to debug port.
#include "../arch/i686/debug.h"
void putc(char c) { i686_debug_putc(c); }

// printf
void _printf_sink(char c, void *ctx) {
  (void)ctx;
  putc(c);
}

uint32_t printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  printf_sink_t sink = {
      .putc_fn = _printf_sink,
      .ctx = NULL,
  };
  uint32_t written = _vprintf_core(fmt, args, &sink);
  va_end(args);
  return written;
}

uint32_t vprintf(const char *fmt, va_list args) {
  printf_sink_t sink = {
      .putc_fn = _printf_sink,
      .ctx = NULL,
  };

  return _vprintf_core(fmt, args, &sink);
}
