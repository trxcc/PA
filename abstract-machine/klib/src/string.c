#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t ans = 0;
  for (size_t i = 0; s[i] != '\0'; ++i, ++ans);
  return ans;
  panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {
  size_t i;
  for (i = 0; src[i] != '\0'; i++) dst[i] = src[i];
  dst[i] = '\0';
  return dst;
  panic("Not implemented");
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++)
    dst[i] = src[i];
  for ( ; i < n; i++)
    dst[i] = '\0';
  return dst;
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  size_t dst_len = strlen(dst);
  size_t i;
  for(i = 0; src[i] != '\0'; i++) dst[dst_len + i] = src[i];
  dst[dst_len + i] = '\0';
  return dst;
  panic("Not implemented");
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 != '\0' && *s2 != '\0') {
    if (*s1 > *s2) return 1;
    else if (*s1 < *s2) return -1; 
    s1 += 1; s2 += 1;
  }
  if (*s1 != '\0') return 1;
  if (*s2 != '\0') return -1;
  return 0;
  panic("Not implemented");
}

int strncmp(const char *s1, const char *s2, size_t n) {
  for (size_t i = 1; i <= n; i++) {
    if (s1[i] < s2[i]) return -1;
    else if (s1[i] > s2[i]) return 1;
  }
  return 0;
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  if (!s || n < 0) return s;
  char *p = (char *) s;
  while (n-- > 0) *p = c;
  return s;
  panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *c1, *c2;
  int res;
  for (c1 = s1, c2 = s2; n > 0; ++c1, ++c2, --n)
    if ((res = *c1 - *c2) != 0)
      break;
  return res;
  panic("Not implemented");
}

#endif
