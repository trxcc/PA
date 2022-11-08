#ifndef __FTRACE_H__
#define __FTRACE_H__

#include <cpu/decode.h>

typedef struct {
  char *name;
  uint32_t start_addr, end_addr;
}FuncNode;

extern char *ftrace_ans;
extern int FTRACE_CNT;
extern FuncNode funcnode[100];
extern void getStrTable(char *filepath);
extern void getFunc();
extern void init_ftrace(char *filepath);
extern void ftrace_record(Decode *s);

#endif
