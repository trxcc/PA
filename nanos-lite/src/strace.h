#ifndef __STRACE_H__
#define __STRACE_H__

typedef struct {
  uint32_t call_type, gpr2, gpr3, gprx;
}strace_node;

#define MAX_NUM_OF_STRACE 5000

#endif
