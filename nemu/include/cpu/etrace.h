#ifndef __ETRACE_H__
#define __ETRACE_H__

#include <cpu/decode.h>
#define MAX_EXCEPTION_NODE 65535

typedef struct {
  word_t MEPC;
  word_t MCAUSE;
  word_t MTVEC;
  word_t MSTATUS;
}exceptionNode;

uint64_t etrace_cnt = 0; 

extern void etrace_exec();
extern void print_etrace();

#endif
