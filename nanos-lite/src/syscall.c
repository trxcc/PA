#include <common.h>
#include "syscall.h"

extern void yield();

static int sys_yield() {
  printf("what\n");
  yield();
  return 0;
}

static void sys_exit(int flag) {
#ifdef CONFIG_STRACE
  print_strace();
  reset_strace();
#endif
  halt(flag);
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  //printf("a[0]: %u", a[0]);
  switch (a[0]) {
    case SYS_yield: 
      c->GPRx = sys_yield(); 
#ifdef CONFIG_STRACE
      strace_record(a, c->GPRx);
#endif
      break;
    case SYS_exit: 
#ifdef CONFIG_STRACE
      strace_record(a, 0);
#endif
      sys_exit(a[0]); 
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
