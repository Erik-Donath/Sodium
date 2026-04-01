#include "stdio.h"
#include "ctype.h"
#include "string.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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

typedef union {
  uint64_t    u;
  int64_t     s;
  long double f;
  void*       p;
} printf_value_t;

typedef enum {
  ARG_SIGNED_INT = 1,
  ARG_UNSIGNED_INT = 2,
  ARG_FLOAT = 3,
  ARG_POINTER = 4,
} printf_arg_type_t;

static printf_value_t _get_argument(va_list args, exec_lenght_t len, printf_arg_type_t type) {
  printf_value_t val = {0};

  switch (type) {
    case ARG_SIGNED_INT:
      switch (len) {
        case LENGHT_SIGNED_CHAR: val.s = (int8_t)va_arg(args, int); break;
        case LENGHT_SHORT:       val.s = (int16_t)va_arg(args, int); break;
        case LENGHT_LONG:        val.s = va_arg(args, long); break;
        case LENGHT_LONGLONG:    val.s = va_arg(args, long long); break;
        case LENGHT_SIZE:        val.s = (int64_t)va_arg(args, size_t); break;
        case LENGHT_INTMAX:      val.s = va_arg(args, intmax_t); break;
        case LENGHT_PTRDIFF:     val.s = va_arg(args, ptrdiff_t); break;
        default:                 val.s = va_arg(args, int); break;
      }
      break;

    case ARG_UNSIGNED_INT:
      switch (len) {
        case LENGHT_SIGNED_CHAR: val.u = (uint8_t)va_arg(args, unsigned int); break;
        case LENGHT_SHORT:       val.u = (uint16_t)va_arg(args, unsigned int); break;
        case LENGHT_LONG:        val.u = va_arg(args, unsigned long); break;
        case LENGHT_LONGLONG:    val.u = va_arg(args, unsigned long long); break;
        case LENGHT_SIZE:        val.u = va_arg(args, size_t); break;
        case LENGHT_INTMAX:      val.u = va_arg(args, uintmax_t); break;
        case LENGHT_PTRDIFF:     val.u = (uint64_t)va_arg(args, ptrdiff_t); break;
        default:                 val.u = va_arg(args, unsigned int); break;
      }
      break;

    case ARG_FLOAT:
      if (len == LENGHT_LONGDOUBLE)
        val.f = va_arg(args, long double);
      else
        val.f = (long double)va_arg(args, double);  // float promotes to double
      break;

    case ARG_POINTER:
      val.p = va_arg(args, void *);
      break;
  }

  return val;
}

// Expects fmt to be at the char behind %.
// Returns fmt after the last char of the expression.
const char *_vprintf_exec(const char *fmt, va_list args, printf_sink_t *sink, uint32_t *written) {
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
  if (*fmt == '%') {
    sink->putc_fn('%', sink->ctx);
    (*written)++;
    return fmt + 1;
  }

  else if (*fmt == 'd' || *fmt == 'i') {
    printf_value_t v = _get_argument(args, lenght, ARG_SIGNED_INT);
    uint64_t absval = (v.s < 0) ? (uint64_t)(-v.s) : (uint64_t)v.s;
    number_type_t t = (v.s < 0) ? NUMBER_DECIMAL_NEGATIV : NUMBER_DECIMAL_POSITIV;
    *written += _print_number(absval, t, false, flags, width, prec, sink);
  }

  else if (*fmt == 'u') {
    printf_value_t v = _get_argument(args, lenght, ARG_UNSIGNED_INT);
    *written += _print_number(v.u, NUMBER_DECIMAL_UNSIGNED, false, flags, width, prec, sink);
  }

  else if (*fmt == 'o') {
    printf_value_t v = _get_argument(args, lenght, ARG_UNSIGNED_INT);
    *written += _print_number(v.u, NUMBER_OCTAL, false, flags, width, prec, sink);
  }

  else if (*fmt == 'x' || *fmt == 'X') {
    printf_value_t v = _get_argument(args, lenght, ARG_UNSIGNED_INT);
    *written += _print_number(v.u, NUMBER_HEX, (*fmt == 'X'), flags, width, prec, sink);
  }

  else if (*fmt == 'p') {
    printf_value_t v = _get_argument(args, LENGHT_NONE, ARG_POINTER);
    *written += _print_number((uintptr_t)v.p, NUMBER_HEX, false, FLAGS_ALTERNATE, width, prec, sink);
  }

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
