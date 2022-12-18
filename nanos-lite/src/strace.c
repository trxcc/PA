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

extern char *fs_get_file_name(int);

static const int file_index[] = {3, 4, 7, 8};

static strace_node S[MAX_NUM_OF_STRACE];
static int strace_cnt = 0, nr_strace = 0;

void strace_record(uintptr_t x[], uint32_t ret) {
  if (x[0] < 0 || x[0] > 19) panic("Unhandled syscall ID = %d", x[0]);
  S[nr_strace].call_type = x[0];
  S[nr_strace].gpr2 = x[1];
  S[nr_strace].gpr3 = x[2];
  S[nr_strace].gpr4 = x[3];
  S[nr_strace].gprx = ret;
  nr_strace = (nr_strace + 1) % MAX_NUM_OF_STRACE;
  ++strace_cnt;  
}

static bool check_file_op(int i) {
  for (int j = 0; j < 4; j++) {
    if (file_index[j] == S[i].call_type) {
      return true;
    }
  }
  return false;
}

void print_strace() {
  printf("\n**********************************STRACE************************************\n");
  printf("\n");
  int max_len = strace_cnt >= MAX_NUM_OF_STRACE ? MAX_NUM_OF_STRACE : strace_cnt;
  int arrow_index = nr_strace == 0 ? MAX_NUM_OF_STRACE : nr_strace - 1;
  for (int i = 0; i < max_len; i++) {
    if (i == arrow_index) printf("   --->   ");
    else                  printf("          ");
    printf("Type: %s\t gpr2: %u\t gpr3: %u\t gpr4: %u\t ret: %u\t", syscall_string[S[i].call_type], S[i].gpr2, S[i].gpr3, S[i].gpr4, S[i].gprx);
    if (S[i].call_type == 2) printf("File: %s", (char *)S[i].gpr2);
    else if (check_file_op(i)) printf("File: %s", fs_get_file_name(S[i].gpr2));
    printf("\n");
  }
  printf("\n"); 
}

void reset_strace() {
  strace_cnt = 0, nr_strace = 0; 
}
