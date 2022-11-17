#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

#define W 400
#define H 300

void __am_gpu_init() {
  int i;
  int w = W;
  int h = H;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i++) fb[i] = i;
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = W, .height = H,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  /*
  for (int i = 0; i < ctl->w * ctl->h; i++) {
    uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
    fb[i] = *((uint32_t*)ctl->pixels + i);
  }*/
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  if(w == 0 || h == 0) return;
  printf("x: %d, y: %d, w: %d, h: %d\n", x, y, w, h);
  int ind = (y) * W + x;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t *suk = (uint32_t *)ctl->pixels;
  for (int i = 0; i < h && y + i < H; i++) {
    for (int j = 0; j < w && x + j < W; j++) {
      printf("fb_index: %d, suk_index: %d\n", ind + (i)*W+j, (i)*w+j);
      fb[ind + (i)*W+j] = suk[(i)*w+j];
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
