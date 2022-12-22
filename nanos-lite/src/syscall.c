#include <common.h>
#include "syscall.h"
#include <sys/time.h>
#include <proc.h>

extern void yield();
extern int fs_open(const char *, int, int);
extern size_t fs_read(int, void *, size_t);
extern size_t fs_write(int, const void *, size_t);
extern size_t fs_lseek(int, size_t, int);
extern int fs_close(int);
extern void naive_uload(PCB *, const char *);

#ifdef CONFIG_STRACE
extern void strace_record(uintptr_t x[], uint32_t ret);
extern void print_strace();
extern void reset_strace();
#endif

static int sys_yield() {
  //printf("what\n");
  yield();
  return 0;
}

static int sys_execve(const char *fname, char * const argv[], char *const envp[]) {
  printf("%s\n", fname);
  naive_uload(NULL, fname);
  return 0;
}

static void sys_exit(int flag) {
#ifdef CONFIG_STRACE
  print_strace();
  reset_strace();
#endif
  sys_execve("/bin/menu", NULL, NULL);
  halt(flag);
}

static int sys_write(int fd, void *buf, size_t count) {
  //Log("Use sys_write\n");
  /*int i = 0;
  if (fd == 1 || fd == 2) {
    char *buff = (char *)buf;
    for (i = 0; i < count; i++) {
      putch(buff[i]);
    }
  }
  else {
    fs_write(fd, buf, count);
  }*/
  return fs_write(fd, buf, count);
} 

static int sys_brk(intptr_t addr){
  //Log("Use sys_brk");
  return 0;
}

static int sys_gettimeofday(struct timeval *tv, struct timezone *tz) {
  tv->tv_usec = io_read(AM_TIMER_UPTIME).us;
  tv->tv_sec  = tv->tv_usec / 1000000;
  if (tv->tv_usec < 0 || tv->tv_sec < 0) return  -1;
  return 0; 
}

void do_syscall(Context *c) {
  uintptr_t a[4];
/*#ifdef CONFIG_STRACE
  uintptr_t x[3];
  x[0] = c->mcause;
  x[1] = c->mepc;
  x[2] = c->mstatus;
#endif*/
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  //printf("a[0]: %u\n", a[0]);
  switch (a[0]) {
    case SYS_yield: 
      c->GPRx = sys_yield(); 
      break;
    case SYS_write:
      //putch('a');
      c->GPRx = sys_write(a[1], (void *)a[2], a[3]);
      break;
    case SYS_brk:
     // putch('h');
      c->GPRx = sys_brk(a[1]);
      break;
    case SYS_open:
      //printf("Hit SYS_open\n");
      c->GPRx = fs_open((char *)a[1], a[2], a[3]);
      break;
    case SYS_read:
      //printf("Hit SYS_read\n");
      //printf("len: %d\n", a[3]);
      c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
      break;
    case SYS_lseek:
      //printf("Hit SYS_lseek\n");
      c->GPRx = fs_lseek(a[1], a[2], a[3]);
      break;
    case SYS_close:
      //printf("Hit SYS_close\n");
      c->GPRx = fs_close(a[1]);
      break;
    case SYS_gettimeofday:
      c->GPRx = sys_gettimeofday((struct timeval *)a[1], (struct timezone *)a[2]); 
      break;
    case SYS_execve:
      c->GPRx = sys_execve((char *)a[1], (char * const*)a[2], (char * const*)a[3]);
      break;
    case SYS_exit: 
#ifdef CONFIG_STRACE
      strace_record(a, 0);
#endif
      sys_exit(a[0]); 
      c->GPRx = 0;
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
#ifdef CONFIG_STRACE
    strace_record(a, c->GPRx);
#endif
}
