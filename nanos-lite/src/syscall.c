#include <common.h>
#include "syscall.h"

extern void yield();

static int sys_yield() {
  printf("what\n");
  yield();
  return 0;
}

static void sys_exit(int flag) {
  halt(flag);
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  //printf("a[0]: %u", a[0]);
  switch (a[0]) {
    case SYS_yield: c->GPRx = sys_yield(); break;
    case SYS_exit: sys_exit(a[0]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
