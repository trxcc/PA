#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define MAX_NUM_IN_OUT 5000
static char printf_out[MAX_NUM_IN_OUT];

int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int len = vsprintf(printf_out, fmt, ap);
  va_end(ap);
  for(int i = 0; i < len; i++) { putch(printf_out[i]); }
  return len;
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int i = 0;
  const char *p = fmt;
  while (*p != '\0') {
    if (*p != '%') {
      out[i++] = *p; ++p;
    }
    else {
      ++p;
      switch (*p) {
        case 'd':
          int vaint = va_arg(ap, int);
          int len = 0; int64_t tmp = vaint;
          if(tmp == 0) {
            out[i++] = '0';
            break;
          }
          else if (tmp < 0) {
            out[i++] = '-';
            tmp = -tmp;
            vaint = -vaint;
          }
          while (tmp) ++len, tmp /= 10;
          for (int j = i + len - 1; j >= i; j--){
            out[j] = (char)((vaint % 10) + '0');
            vaint /= 10;
          } 
          i += len; break;
        case 's':
          char *vachars = va_arg(ap, char *);
          while (*vachars != '\0') {
            out[i++] = *vachars;
            vachars++;
          }
          break;
        default: out[i++] = *p; break;
      }
      ++p;
    }
  }
  out[i] = '\0';
  return i;
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  va_list vl;
  va_start(vl, fmt);
  int ret = vsprintf(out, fmt, vl);
  va_end(vl);
  return ret;
  panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
