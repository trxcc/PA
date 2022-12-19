#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

static long int BOOT_TIME = 0;

uint32_t NDL_GetTicks() {
  struct timeval val;
  gettimeofday(&val, NULL);
  long int now_time = val.tv_sec;
  return (uint32_t)(now_time - BOOT_TIME);
}

int NDL_PollEvent(char *buf, int len) {
  FILE *fp = fopen("/dev/events", "r+");
  char *c;
  int i = fread(c, sizeof(char), len, fp);
  //assert(0);
  printf("%s\n", c);
  if (i == 0) return 0; 
  memcpy(buf, c, len);
  return 1;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  struct timeval val;
  gettimeofday(&val, NULL);
  BOOT_TIME = val.tv_sec;
  return 0;
}

void NDL_Quit() {
  BOOT_TIME = 0;
}
