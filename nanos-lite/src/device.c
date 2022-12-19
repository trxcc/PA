#include <common.h>
#include <stdio.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  char *buff = (char *)buf;
  for (size_t i = 0; i < len; i++) {
    putch(buff[i]);
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  
  if (ev.keycode == AM_KEY_NONE) return 0;
  char *buff = (char *)buf;
  *buff++ = 'k';
  *buff++ = ev.keydown ? 'd' : 'u';
  *buff++ = ' ';
  
  int keylen = strlen(keyname[ev.keycode]);
  printf("%s\n", keyname[ev.keycode]);
  memcpy(buff, keyname[ev.keycode], keylen < len ? keylen : len);
  buff += (keylen < len ? keylen : len);
  *buff = '\n';
  return keylen + 4;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
