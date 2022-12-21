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

#define N 1000
//static int in_canvas[400][300];
static int canvas_w = 0, canvas_h = 0;

uint32_t NDL_GetTicks() {
  struct timeval val;
  gettimeofday(&val, NULL);
  long int now_time = val.tv_sec;
  return (uint32_t)(now_time - BOOT_TIME);
}

int NDL_PollEvent(char *buf, int len) {
  FILE *fp = fopen("/dev/events", "r+");
  int i = fread(buf, sizeof(char), len, fp);
  //assert(0);
  if (i == 0) return 0; 
  return 1;
}

void NDL_OpenCanvas(int *w, int *h) {
  printf("*w: %d, *h: %d\n", *w, *h);
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
  else {
    FILE *fp = fopen("/proc/dispinfo", "r+");
    char *tmp_str1 = "i am the king\n", *tmp_str2 = "lsy\n";
    fscanf(fp, "%s %d %s %d", tmp_str1, &screen_w, tmp_str2, &screen_h);
    printf("SCREEN_W: %d, SCREEN_H: %d\n", screen_w, screen_h);
    if (*w == 0 && *h == 0) {
      *w = screen_w, *h = screen_h;
    }
    assert(*w <= screen_w);
    assert(*h <= screen_h);
    canvas_w = *w <= screen_w ? *w : screen_w, canvas_h = *h <= screen_h ? *h : screen_h;
   /* for (int i = 0; i < *w; i++) 
      for (int j = 0; j < *h; j++)
        in_canvas[i][j] = 1;*/
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  printf("x: %d, y: %d, w: %d, h: %d, c_w: %d, c_h: %d, s_w: %d, s_h: %d\n", x, y, w, h, canvas_w, canvas_h, screen_w, screen_h);
  FILE* fp = fopen("/dev/fb", "r+");
  assert(x + w - 1 < 400);
  assert(y + h - 1 < 300);
  //assert(in_canvas[x + w - 1][y + h - 1] != 0);
  fseek(fp, 0, SEEK_SET);
  
  int start_y = (screen_h - canvas_h) / 2, start_x = (screen_w - canvas_w) / 2;
  fseek(fp, (start_y * screen_w + start_x) * 4, SEEK_SET);
  fseek(fp, (y * screen_w + x) * 4, SEEK_CUR);
  printf("start_x: %d, start_y: %d, x: %d, y: %d, w: %d, h: %d\n", start_x, start_y, x, y, w, h);
  for (int j = 0; j < h; j++) {
    //printf("lines: %d, len: %d\n", start_y + y + j, w);
    fseek(fp, ((start_y + y + j) * screen_w + start_x) * 4, SEEK_SET);
    fwrite(pixels + j * w, sizeof(uint32_t), w, fp);
  }
  fseek(fp, 0, SEEK_SET);
  //printf("hi\n");
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
  //memset(in_canvas, 0, sizeof(in_canvas));
  //for (int i = 0; i < 400; i++)
  //  for (int j = 0; j < 300; j++)
  //    in_canvas[i][j] = 0;
  return 0;
}

void NDL_Quit() {
  BOOT_TIME = 0;
  //memset(in_canvas, 0, sizeof(in_canvas));
  //for (int i = 0; i < 400; i++)
  //  for (int j = 0; j < 300; j++)
  //    in_canvas[i][j] = 0;
}
