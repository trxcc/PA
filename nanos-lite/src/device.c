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
  keylen = keylen < len ? keylen : len;
  memcpy(buff, keyname[ev.keycode], keylen < len ? keylen : len);
  buff += keylen;
  //*buff++ = '\n';
  *buff = '\0';
  //printf("%s\n", (char *)buf);
  return keylen + 4;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  char *buff = (char *)buf;
  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;
  len = len < 30 ? len : 30;
  sprintf(buff, "WIDTH: %d\nHEIGHT: %d\n", w, h);  
  buff += len;
  *buff = '\0';
  printf("%s\n", (char *)buf);
  return len;
}

#define W 400
#define H 300
static uint32_t color_buf[W * H];

size_t fb_write(const void *buf, size_t offset, size_t len) {
  assert(offset <= W * H);
  assert(0);
  int y = offset / W, x = offset - y * W;
  uint32_t *buff = (uint32_t *)buf;
  for (size_t i = 0; i < len; i++) {
    color_buf[y * W + x + i] = buff[i];
  }
  for (int i = 0; i < W * H; i++) {color_buf[i] = i % 255; printf("i: %d, color: %d\n", i, color_buf[i]);}
  io_write(AM_GPU_FBDRAW, 0, 0, color_buf, 400, 300, true);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
