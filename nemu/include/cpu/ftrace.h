#ifndef __FTRACE_H__
#define __FTRACE_H__

typedef struct {
  char *name;
  uint32_t start_addr, end_addr;
}FuncNode;

extern FuncNode funcnode[100];
extern void getStrTable(char *filepath);
extern void getFunc();
extern void init_ftrace(char *filepath);

#endif
