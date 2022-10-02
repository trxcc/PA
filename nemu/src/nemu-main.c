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
#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

char *FILEPATH = "/home/trxcc/ics2022/nemu/tools/gen-expr/input_cc";

extern word_t expr(char *e, bool success);

void compare(){
  FILE *fp = fopen(FILEPATH, "r");
  assert(fp != NULL);
  char *tmp = NULL;// word_t ans;
  //bool flag = true;
  int f = fscanf(fp, "%s", tmp);
  assert(tmp != NULL);
  printf("%d\n", f);
  //printf("%u\n", expr(e, flag));
  printf("%s\n", tmp);
  fclose(fp);
}

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  compare();
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
