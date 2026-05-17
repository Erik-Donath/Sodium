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
  FLAGS_NONE      = 0x00,   // none
  FLAGS_ALIGN     = 0x01,   // -
  FLAGS_SIGN      = 0x02,   // +
  FLAGS_SPACE     = 0x04,   // space
  FLAGS_PAD       = 0x08,   // 0
  FLAGS_ALTERNATE = 0x10,   // #
} exec_flags_t;

typedef enum exec_length : uint8_t {
  LENGTH_NONE    = 0,        // not given
  LENGTH_SHORT   = 1,        // h
  LENGTH_SCHAR   = 2,        // hh
  LENGTH_LONG    = 3,        // l
  LENGTH_LLONG   = 4,        // ll
  LENGTH_LDOUBLE = 5,        // L
  LENGTH_SIZE    = 6,        // z
  LENGTH_IMAX    = 7,        // j
  LENGTH_PDIFF   = 8,        // t
} exec_length_t;

typedef enum number_type : uint8_t {
  NUMBER_DECIMAL_POS = 0,   // %d positive
  NUMBER_DECIMAL_NEG = 1,   // %d negative
  NUMBER_UNSIGNED    = 2,   // %u
  NUMBER_OCTAL       = 3,   // %o
  NUMBER_HEX         = 4,   // %x / %X
  NUMBER_BINARY      = 5,   // %b / %B
} number_type_t;

static const uint8_t _radix[6]   = { 10, 10, 10, 8, 16, 2 };
static const char    _digits[16] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };

// Resolved argument type: length modifier and conversion kind combined.
typedef enum va_type : uint8_t {
  VA_INT,      // int
  VA_UINT,     // unsigned int
  VA_LONG,     // long
  VA_ULONG,    // unsigned long
  VA_LLONG,    // long long
  VA_ULLONG,   // unsigned long long
  VA_SHORT,    // short (promoted as int)
  VA_USHORT,   // unsigned short (promoted as unsigned int)
  VA_SCHAR,    // signed char (promoted as int)
  VA_UCHAR,    // unsigned char (promoted as unsigned int)
  VA_SIZE,     // size_t
  VA_IMAX,     // intmax_t
  VA_UMAX,     // uintmax_t
  VA_PDIFF,    // ptrdiff_t
  VA_DOUBLE,   // double
  VA_LDOUBLE,  // long double
  VA_PTR,      // void *
} va_type_t;

typedef union {
  uint64_t    u;
  int64_t     s;
  long double f;
  void       *p;
} printf_value_t;

static printf_value_t _pop_arg(va_list *args, va_type_t type) {
  printf_value_t v = {0};
  switch (type) {
    case VA_INT:     v.s = va_arg(*args, int);                     break;
    case VA_UINT:    v.u = va_arg(*args, unsigned int);            break;
    case VA_LONG:    v.s = va_arg(*args, long);                    break;
    case VA_ULONG:   v.u = va_arg(*args, unsigned long);           break;
    case VA_LLONG:   v.s = va_arg(*args, long long);               break;
    case VA_ULLONG:  v.u = va_arg(*args, unsigned long long);      break;
    case VA_SHORT:   v.s = (int16_t)va_arg(*args, int);            break;
    case VA_USHORT:  v.u = (uint16_t)va_arg(*args, unsigned int);  break;
    case VA_SCHAR:   v.s = (int8_t)va_arg(*args, int);             break;
    case VA_UCHAR:   v.u = (uint8_t)va_arg(*args, unsigned int);   break;
    case VA_SIZE:    v.u = va_arg(*args, size_t);                  break;
    case VA_IMAX:    v.s = va_arg(*args, intmax_t);                break;
    case VA_UMAX:    v.u = va_arg(*args, uintmax_t);               break;
    case VA_PDIFF:   v.s = va_arg(*args, ptrdiff_t);               break;
    case VA_DOUBLE:  v.f = (long double)va_arg(*args, double);     break;
    case VA_LDOUBLE: v.f = va_arg(*args, long double);             break;
    case VA_PTR:     v.p = va_arg(*args, void *);                  break;
  }
  return v;
}

static va_type_t _signed_type(exec_length_t len) {
  switch (len) {
    case LENGTH_SCHAR: return VA_SCHAR;
    case LENGTH_SHORT: return VA_SHORT;
    case LENGTH_LONG:  return VA_LONG;
    case LENGTH_LLONG: return VA_LLONG;
    case LENGTH_SIZE:  return VA_PDIFF;
    case LENGTH_IMAX:  return VA_IMAX;
    case LENGTH_PDIFF: return VA_PDIFF;
    default:           return VA_INT;
  }
}

static va_type_t _unsigned_type(exec_length_t len) {
  switch (len) {
    case LENGTH_SCHAR: return VA_UCHAR;
    case LENGTH_SHORT: return VA_USHORT;
    case LENGTH_LONG:  return VA_ULONG;
    case LENGTH_LLONG: return VA_ULLONG;
    case LENGTH_SIZE:  return VA_SIZE;
    case LENGTH_IMAX:  return VA_UMAX;
    case LENGTH_PDIFF: return VA_SIZE;
    default:           return VA_UINT;
  }
}

static uint32_t _emit_padded(printf_sink_t *sink, const char *str, uint32_t len, uint32_t width, bool left) {
  uint32_t spaces = (width > len) ? (width - len) : 0;
  if (!left) for (uint32_t i = 0; i < spaces; i++)
    sink->putc_fn(' ', sink->ctx);
  for (uint32_t i = 0; i < len; i++)
    sink->putc_fn(str[i], sink->ctx);
  if  (left) for (uint32_t i = 0; i < spaces; i++)
    sink->putc_fn(' ', sink->ctx);
  return len + spaces;
}

// prec = -1 => no precision given
// width = 0 => no width given
static uint32_t _print_number(uint64_t value, number_type_t type, bool uppercase, exec_flags_t flags, uint32_t width, int32_t prec, printf_sink_t *sink) {
  char    strnum[64];
  uint8_t strnum_count = 0;
  uint8_t radix = _radix[type];

  if (prec == 0 && value == 0) {
    strnum_count = 0;
  } else if (value == 0) {
    strnum[strnum_count++] = '0';
  } else {
    uint64_t v = value;
    while (v) {
      char digit = _digits[v % radix];
      strnum[strnum_count++] = (uppercase && digit >= 'a') ? (char)toupper(digit) : digit;
      v /= radix;
    }
  }

  const char *prefix = NULL;
  if      (type == NUMBER_DECIMAL_POS && (flags & FLAGS_SIGN))                    prefix = "+";
  else if (type == NUMBER_DECIMAL_POS && (flags & FLAGS_SPACE))                   prefix = " ";
  else if (type == NUMBER_DECIMAL_NEG)                                            prefix = "-";
  else if (type == NUMBER_OCTAL       && (flags & FLAGS_ALTERNATE))               prefix = "0";
  else if (type == NUMBER_HEX         && (flags & FLAGS_ALTERNATE) && !uppercase) prefix = "0x";
  else if (type == NUMBER_HEX         && (flags & FLAGS_ALTERNATE) &&  uppercase) prefix = "0X";
  else if (type == NUMBER_BINARY      && (flags & FLAGS_ALTERNATE) && !uppercase) prefix = "0b";
  else if (type == NUMBER_BINARY      && (flags & FLAGS_ALTERNATE) &&  uppercase) prefix = "0B";

  uint32_t prefix_len  = prefix ? (uint32_t)strlen(prefix) : 0;
  uint32_t digit_zeros = (prec > 0 && (uint32_t)prec > strnum_count) ? ((uint32_t)prec - strnum_count) : 0;
  uint32_t content     = prefix_len + digit_zeros + strnum_count;
  uint32_t spaces      = (width > content) ? (width - content) : 0;

  bool left_align = (flags & FLAGS_ALIGN) != 0;
  bool zero_pad   = (flags & FLAGS_PAD) && !left_align && (prec < 0);

  if (!left_align) {
    if (zero_pad) {
      if (prefix) for (uint32_t i = 0; prefix[i]; i++) sink->putc_fn(prefix[i], sink->ctx);
      for (uint32_t i = 0; i < spaces; i++) sink->putc_fn('0', sink->ctx);
    } else {
      for (uint32_t i = 0; i < spaces; i++) sink->putc_fn(' ', sink->ctx);
      if (prefix) for (uint32_t i = 0; prefix[i]; i++) sink->putc_fn(prefix[i], sink->ctx);
    }
  } else if (prefix) {
    for (uint32_t i = 0; prefix[i]; i++) sink->putc_fn(prefix[i], sink->ctx);
  }

  for (uint32_t i = 0; i < digit_zeros; i++)             sink->putc_fn('0', sink->ctx);
  for (int8_t i = (int8_t)strnum_count - 1; i >= 0; i--) sink->putc_fn(strnum[i], sink->ctx);

  if (left_align) {
    for (uint32_t i = 0; i < spaces; i++) sink->putc_fn(' ', sink->ctx);
  }

  return content + spaces;
}

// Expects fmt to be at the char behind %.
// Returns fmt after the last char of the expression.
static const char *_vprintf_exec(const char *fmt, va_list *args, printf_sink_t *sink, uint32_t *written) {
  exec_flags_t  flags  = FLAGS_NONE;
  exec_length_t length = LENGTH_NONE;
  uint32_t      width  = 0;
  int32_t       prec   = -1;

  if (*fmt == '\0') return fmt;

  // Flag state
  {
    bool scanning = true;
    while (scanning) {
      switch (*fmt) {
        case '-': flags |= FLAGS_ALIGN;     fmt++; break;
        case '+': flags |= FLAGS_SIGN;      fmt++; break;
        case ' ': flags |= FLAGS_SPACE;     fmt++; break;
        case '0': flags |= FLAGS_PAD;       fmt++; break;
        case '#': flags |= FLAGS_ALTERNATE; fmt++; break;
        default:  scanning = false;                break;
      }
    }
  }

  // Width state
  if (*fmt == '*') {
    int w = va_arg(*args, int);
    if (w < 0) { flags |= FLAGS_ALIGN; width = (uint32_t)(-w); }
    else        { width = (uint32_t)w; }
    fmt++;
  } else {
    while (*fmt && isdigit(*fmt)) { width = width * 10 + (uint32_t)(*fmt - '0'); fmt++; }
  }

  // Precision state
  if (*fmt == '.') {
    prec = 0;
    fmt++;
    if (*fmt == '*') {
      int p = va_arg(*args, int);
      prec = (p < 0) ? -1 : p;
      fmt++;
    } else {
      while (*fmt && isdigit(*fmt)) { prec = prec * 10 + (*fmt - '0'); fmt++; }
    }
  }

  if (*fmt == '\0') return fmt;

  // Length state
  switch (*fmt) {
    case 'h': length = (*(fmt+1) == 'h') ? LENGTH_SCHAR : LENGTH_SHORT;  fmt += (*(fmt+1) == 'h') ? 2 : 1; break;
    case 'l': length = (*(fmt+1) == 'l') ? LENGTH_LLONG : LENGTH_LONG;   fmt += (*(fmt+1) == 'l') ? 2 : 1; break;
    case 'L': length = LENGTH_LDOUBLE; fmt++; break;
    case 'z': length = LENGTH_SIZE;    fmt++; break;
    case 'j': length = LENGTH_IMAX;    fmt++; break;
    case 't': length = LENGTH_PDIFF;   fmt++; break;
  }

  if (*fmt == '\0') return fmt;

  // Specifier state
  switch (*fmt) {
    case '%': {
      sink->putc_fn('%', sink->ctx);
      (*written)++;
    } break;
    case 'c': {
      char ch = (char)(unsigned char)va_arg(*args, int);
      *written += _emit_padded(sink, &ch, 1, width, (flags & FLAGS_ALIGN) != 0);
    } break;
    case 's': {
      printf_value_t v = _pop_arg(args, VA_PTR);
      const char *str = v.p ? (const char *)v.p : "(null)";
      uint32_t len = 0;
      if (prec >= 0) {
        while (len < (uint32_t)prec && str[len])
          len++;
      }
      else {
        while (str[len])
          len++;
      }
      *written += _emit_padded(sink, str, len, width, (flags & FLAGS_ALIGN) != 0);
    } break;
    case 'd':
    case 'i': {
      printf_value_t v = _pop_arg(args, _signed_type(length));
      uint64_t absval = (v.s < 0) ? ((uint64_t)0 - (uint64_t)v.s) : (uint64_t)v.s;
      number_type_t t = (v.s < 0) ? NUMBER_DECIMAL_NEG : NUMBER_DECIMAL_POS;
      *written += _print_number(absval, t, false, flags, width, prec, sink);
    } break;
    case 'u': {
      printf_value_t v = _pop_arg(args, _unsigned_type(length));
      *written += _print_number(v.u, NUMBER_UNSIGNED, false, flags, width, prec, sink);
    } break;
    case 'o': {
      printf_value_t v = _pop_arg(args, _unsigned_type(length));
      *written += _print_number(v.u, NUMBER_OCTAL, false, flags, width, prec, sink);
    } break;
    case 'x':
    case 'X': {
      printf_value_t v = _pop_arg(args, _unsigned_type(length));
      *written += _print_number(v.u, NUMBER_HEX, (*fmt == 'X'), flags, width, prec, sink);
    } break;
    case 'b':
    case 'B': {
      printf_value_t v = _pop_arg(args, _unsigned_type(length));
      *written += _print_number(v.u, NUMBER_BINARY, (*fmt == 'B'), flags, width, prec, sink);
    } break;
    case 'p': {
      printf_value_t v = _pop_arg(args, VA_PTR);
      if (v.p == NULL) {
        *written += _emit_padded(sink, "(nil)", 5, width, (flags & FLAGS_ALIGN) != 0);
      } else {
        exec_flags_t p_flags = (exec_flags_t)(flags | FLAGS_ALTERNATE);
        *written += _print_number((uintptr_t)v.p, NUMBER_HEX, false, p_flags, width, prec, sink);
      }
    } break;
    default:
      break;
  }

  return fmt + 1;
}

static uint32_t _vprintf_core(const char *fmt, va_list *args, printf_sink_t *sink) {
  uint32_t written = 0;

  while (*fmt) {
    if (*fmt == '%') {
      fmt = _vprintf_exec(fmt + 1, args, sink, &written);
    } else {
      sink->putc_fn(*fmt++, sink->ctx);
      written++;
    }
  }

  return written;
}

// #FIXME: Temporarly defining putc to output to debug port.
#include "../arch/i686/debug.h"
void putc(char c) { i686_debug_putc(c); }

static void _printf_putc(char c, void *ctx) {
  (void)ctx;
  putc(c);
}

uint32_t printf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  printf_sink_t sink = { .putc_fn = _printf_putc, .ctx = NULL };
  uint32_t written = _vprintf_core(fmt, &args, &sink);
  va_end(args);
  return written;
}

uint32_t vprintf(const char *fmt, va_list *args) {
  printf_sink_t sink = { .putc_fn = _printf_putc, .ctx = NULL };
  return _vprintf_core(fmt, args, &sink);
}

typedef struct {
  char   *buf;
  size_t  pos;
  size_t  max;  // 0 = sprintf (no limit)
} snprintf_ctx_t;

static void _snprintf_putc(char c, void *ctx) {
  snprintf_ctx_t *s = (snprintf_ctx_t *)ctx;
  if (s->max == 0) {
    s->buf[s->pos] = c;
  } else {
    if (s->pos + 1 < s->max)
      s->buf[s->pos] = c;
  }
  s->pos++;
}

uint32_t vsprintf(char *buf, const char *fmt, va_list *args) {
  snprintf_ctx_t sctx = { .buf = buf, .pos = 0, .max = 0 };
  printf_sink_t  sink = { .putc_fn = _snprintf_putc, .ctx = &sctx };
  uint32_t written = _vprintf_core(fmt, args, &sink);
  buf[sctx.pos] = '\0';
  return written;
}

uint32_t sprintf(char *buf, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  uint32_t written = vsprintf(buf, fmt, &args);
  va_end(args);
  return written;
}

uint32_t vsnprintf(char *buf, size_t size, const char *fmt, va_list *args) {
  if (size == 0) {
    char dummy[1];
    snprintf_ctx_t sctx = { .buf = dummy, .pos = 0, .max = 1 };
    printf_sink_t  sink = { .putc_fn = _snprintf_putc, .ctx = &sctx };
    return _vprintf_core(fmt, args, &sink);
  }

  snprintf_ctx_t sctx = { .buf = buf, .pos = 0, .max = size };
  printf_sink_t  sink = { .putc_fn = _snprintf_putc, .ctx = &sctx };
  uint32_t written = _vprintf_core(fmt, args, &sink);

  buf[sctx.pos < size ? sctx.pos : size - 1] = '\0';
  return written;
}

uint32_t snprintf(char *buf, size_t size, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  uint32_t written = vsnprintf(buf, size, fmt, &args);
  va_end(args);
  return written;
}
