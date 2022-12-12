#include <common.h>
#include "syscall.h"

#define SYS_yield 1 

extern void yield();

static int sys_yield() {
  printf("what\n");
  yield();
  return 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  //printf("a[0]: %u", a[0]);
  switch (a[0]) {
    case SYS_yield: c->GPRx = sys_yield(); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
