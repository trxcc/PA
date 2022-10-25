#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)


int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  va_list vl;
  va_start(vl, fmt);
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
          int vaint = va_arg(vl, int);
          int len = 0, tmp = vaint;
          while (tmp) ++len, tmp /= 10;
          for (int j = i + len - 1; j >= i; j--){
            out[j] = (char)((vaint % 10) + '0');
            vaint /= 10;
          } 
          i += len; break;
        case 's':
          char *vachars = va_arg(vl, char *);
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
  va_end(vl);
  out[i] = '\0';
  return i;
  panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
