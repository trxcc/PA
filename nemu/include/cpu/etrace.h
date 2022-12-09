#ifndef __ETRACE_H__
#define __ETRACE_H__

#include <cpu/decode.h>
#define MAX_EXCEPTION_NODE 20

typedef struct {
  word_t MEPC;
  word_t MCAUSE;
  word_t MTVEC;
  word_t MSTATUS;
}exceptionNode;


extern void etrace_exec();
extern void print_etrace();

#endif
