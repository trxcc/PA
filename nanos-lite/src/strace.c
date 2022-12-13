#include <common.h>
#include "strace.h"

static const char *syscall_string[] = {
  "SYS_exit",
  "SYS_yield",
  "SYS_open",
  "SYS_read",
  "SYS_write",
  "SYS_kill",
  "SYS_getpid",
  "SYS_close",
  "SYS_lseek",
  "SYS_brk",
  "SYS_fstat",
  "SYS_time",
  "SYS_signal",
  "SYS_execve",
  "SYS_fork",
  "SYS_link",
  "SYS_unlink",
  "SYS_wait",
  "SYS_times",
  "SYS_gettimeofday"
};

static strace_node S[MAX_NUM_OF_STRACE];
static int strace_cnt = 0, nr_strace = 0;

void strace_record(uintptr_t x[], uint32_t ret) {
  if (x[0] < 0 || x[0] > 19) panic("Unhandled syscall ID = %d", x[0]);
  S[nr_strace].call_type = x[0];
  S[nr_strace].gpr2 = x[1];
  S[nr_strace].gpr3 = x[2];
  S[nr_strace].gprx = ret;
  nr_strace = (nr_strace + 1) % MAX_NUM_OF_STRACE;
  ++strace_cnt;  
}

void print_strace() {
  printf("\n");
  int max_len = strace_cnt >= MAX_NUM_OF_STRACE ? MAX_NUM_OF_STRACE : strace_cnt;
  int arrow_index = nr_strace == 0 ? MAX_NUM_OF_STRACE : nr_strace - 1;
  for (int i = 0; i < max_len; i++) {
    if (i == arrow_index) printf("   --->   ");
    else                  printf("          ");
    printf("Type: %s\t mepc: %u\t mstatus: %u\n", syscall_string[S[i].call_type], S[i].gpr2, S[i].gpr3, S[i].gprx);
  } 
}

void reset_strace() {
  strace_cnt = 0, nr_strace = 0; 
}
