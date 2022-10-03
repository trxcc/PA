/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
// #include <memory/vaddr.h>

extern uint32_t vaddr_read(vaddr_t addr, int len);
static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_p(char *args) {
  //char *arg = strtok(NULL, " ");
  bool flag = true;
  word_t ans = expr(args, &flag);
  Log("%u\n", ans);
  return 0;  
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){
  //printf("%ld %s\n", strlen(args), args);
  char *arg = strtok(NULL, " ");
  int ans = 0;
  if (arg == NULL) ans = 1;
  else{ 
    int n = strlen(arg); ans = 0;
    for (int i = 0; i < n; i++){
      if (arg[i] <= '0' || arg[i] >= '9') {
        printf("Illegal input. 'si' shoule be followed by numbers.\n");
        return 0;
      } 
      ans = ans * 10 + (arg[i] - '0');
    }
  }
  //Log("%d\n", ans);
  cpu_exec(ans);
  return 0; 
}

static int cmd_info(char *args){
  char *arg = strtok(NULL, " ");
  if(arg == NULL){
    Log("Necessary arguments required. Type 'help' for more information."); 
    return 0; 
  }
  int n = strlen(arg);
  if (n == 1){
    //printf("%c\n", args[0]);
    switch(*arg){
      case 'r': isa_reg_display(); break;    // 114 is the ASCII code of "r"
      case 'w': 
      default: break;
    }
  }
  return 0;
}

static int cmd_x(char *args){
  void print(){
    printf("Necessary arguments required. Type 'help' for more information.\n");
    return;
  }
  if (args == NULL) { print(); return 0; }
  char *arg = strtok(args, " ");
  if (arg == NULL) { print(); return 0; }

  int N = 0;
  for (int i = 0; i < strlen(arg); i++){
    if (arg[i] <= '0' && arg[i] >= '9') {
      printf("Illegal input\n");
      return 0;
    }
    N = N * 10 + arg[i] - '0';
  } 
  
  char *EXPR = strtok(NULL, "\n");
  /*char *str;*/ bool tmp = true;
  //vaddr_t addr = strtol(EXPR, &str, 16);
  vaddr_t addr = expr(EXPR, &tmp);
  printf("%d\n", addr);
  for (int i = 0; i < N; i++){
    uint32_t data = vaddr_read(addr + i*4, 4);
    printf("0x%08X:   ", addr + i*4);
    for (int j = 0; j < 4; ++j){
      printf("%02X ", data & 0xff);
      data >>= 8;  
    }
    printf("\n");
  }
  return 0;
}

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Execute for a single step", cmd_si},
  { "info", "Display information about the memories or registers", cmd_info },
  { "x", "Scan and print the value of the memories", cmd_x },
  { "p", "Print the value of expression", cmd_p},
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
