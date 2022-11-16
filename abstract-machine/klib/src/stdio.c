#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define MAX_NUM_IN_OUT 5000
static char printf_out[MAX_NUM_IN_OUT];

#define is_num(x) x >= '0' && x <= '9'
/*
static struct form_flag{
  bool tal_flag;
  bool d_flag;
  bool zero_flag;
};*/

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
  int i = 0, d_len = 0;
  bool d_flag = false;
  const char *p = fmt;
  while (*p != '\0') {
    if (*p != '%') {
      out[i++] = *p; ++p;
    }
    else {
      ++p;
      while (is_num(*p)) {
        d_len = d_len * 10 + *p - '0';
        d_flag = true;
        p++;
      }
      if (*p == 'd') {
        int vaint = va_arg(ap, int);
        int len = 0, tmp = vaint;
        uint32_t valint = vaint, tmp1 = vaint;
        if(tmp == 0) {
          if (d_flag) {
            while(d_len--) { out[i++] = '0'; }
            d_len = 0, d_flag = false;
          }
          else { out[i++] = '0'; }
        }
        else if (tmp < 0 && !d_flag) {
          out[i++] = '-';
          tmp1 = -tmp;
          valint = -vaint;
        }
        while (tmp1) ++len, tmp1 /= 10;
        if (d_flag) {
          if (len < d_len) {
            for (int j = 0; j < d_len - len; j++) {
              out[i+j] = '0';
            }
            i += (d_len - len);
          }
          d_len = 0;
          d_flag = false;
        }
        for (int j = i + len - 1; j >= i; j--){
          out[j] = (char)((valint % 10) + '0');
          valint /= 10;
        } 
        i += len;
        
      }
      else if (*p == 's') {
        if (d_flag) { d_flag = false, d_len = 0; }
        char *vachars = va_arg(ap, char *);
        while (*vachars != '\0') {
          out[i++] = *vachars;
          vachars++;
        }
      }
      else if (*p == 'c') {
        if (d_flag) { d_flag = false, d_len = 0; }
        char va_ch = (char)va_arg(ap, int);
        out[i++] = va_ch;
      }
      else out[i++] = *p;
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
