#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  kbd->keydown = 1;
  kbd->keycode = inl(KBD_ADDR);
  printf("keycode: %d\n", kbd->keycode);
}
