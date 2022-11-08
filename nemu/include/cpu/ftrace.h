#ifndef __FTRACE_H__
#define __FTRACE_H__

#include <cpu/decode.h>

#define MAX_FUNC_NODE 500
#define MAX_TRACE_NODE 50000

typedef struct {
  char *name;
  uint32_t start_addr, end_addr;
}FuncNode;

typedef struct {
  char *name;
  uint32_t pc;
  int Type;
  uint32_t jmpAddr;
}TraceNode;

extern int FTRACE_CNT, TRACE_NODE_CNT;
extern FuncNode funcnode[MAX_FUNC_NODE];
extern TraceNode tracenode[MAX_TRACE_NODE];
extern void getStrTable(char *filepath);
extern void getFunc();
extern void init_ftrace(char *filepath);
extern void ftrace_record(Decode *s);

#endif
