#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct printf_sink {
  void (*putc_fn)(char c, void *ctx);
  void *ctx;
} printf_sink_t;

// Expects fmt to be at the char behind %.
// Returns fmt after the last char of the expression.
const char *_vprintf_exec(const char *fmt, va_list args, printf_sink_t *sink, uint32_t *written) {
  (void)args;

  typedef enum exec_flags : uint8_t {
    FLAGS_NONE = 0x00,      // Not given
    FLAGS_ALIGIN = 0x01,    // -
    FLAGS_SIGN = 0x02,      // +
    FLAGS_POSITIV = 0x04,   // space
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

  exec_flags_t flags = FLAGS_NONE;
  exec_lenght_t lenght = LENGHT_NONE;
  uint32_t width = 0;
  uint32_t prec = -1;

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
        flags |= FLAGS_POSITIV;
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
    return ++fmt;
  }

  return fmt;
}

uint32_t _vprintf_core(const char *fmt, va_list args, printf_sink_t *sink) {
  uint32_t written = 0;

  while (*fmt) {
    if (*fmt == '%') {
      fmt = _vprintf_exec(++fmt, args, sink, &written);
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
  printf_sink sink = {
      .putc_fn = _printf_sink,
      .ctx = NULL,
  };
  uint32_t written = _vprintf_core(fmt, args, &sink);
  va_end(args);
  return written;
}

uint32_t vprintf(const char *fmt, va_list args) {
  printf_sink sink = {
      .putc_fn = _printf_sink,
      .ctx = NULL,
  };

  return _vprintf_core(fmt, args, &sink);
}
