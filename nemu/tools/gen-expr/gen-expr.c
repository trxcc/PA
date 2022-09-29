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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = (unsigned)%s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}" ;

int pos = 0;
int token_len = 0;

static uint32_t choose(uint32_t n) {
  return rand() % n;
}

static void gen_num() {
  int num = rand() % 3500, rec = num;
  char tmp[5], e[5]; int i = 0; 
  do{
    tmp[i++] = (char)((num % 10) + '0');
    //printf("%d %d\n", i, num);
    num /= 10;
  }while(num);
  for (int j = 0; j < i; j++) { e[j] = tmp[i-1-j]; }
  //printf("%d %s\n", rec, e);
  strncpy(&buf[pos], &e[0], strlen(e));
  //printf("%d - %ld  %s\n", pos, pos+strlen(e), e);
  pos += strlen(e);
}

static void gen(char ch){
  buf[pos++] = ch;
  //printf("%d %c\n", pos-1, ch);
  return;
}

static void gen_rand_op(){
  char op[] = {'+', '-', '*', '/'};
  buf[pos++] = op[choose(4)];
  //printf("%d %c\n", pos-1, buf[pos-1]);
  return;
}

static int chk(){
  printf("%s\n", buf);
  for (int i = 0; i < pos; i++){
    if (buf[i] == '/' || buf[i] == '*' || buf[i] == '+' || buf[i] == '-') {
      int k = i + 1;
      while(buf[k] == ' ') ++k;
      if (i == pos - 1 || buf[k] == '\0' || buf[k] == ')' || buf[k] == '+' || buf[k] == '-' || buf[k] == '*' || buf[k] == '/') { 
        if (i == pos-1) { buf[i] = '\0'; }
        else for (int j = i; j < pos-1; j++) { buf[j] = buf[j+1]; }
        buf[pos-1] = '\0'; --pos; --i;
      }
    }
    //if (buf[i] == '/') printf("%c %c %d\n", buf[i], buf[i+1], i == pos-1);
  }
  return 1;
}

static void gen_space(){
  int num = rand() % 4;
  for (int i = 0; i < num; i++) { buf[pos+i] = ' ';/* printf("%d is space \n", pos+i);*/ }
  pos += num;
  return;
}

static void gen_rand_expr(int flag) {
  //buf[0] = '\0';
  if (flag) {
    do{
      memset(buf, '\0', sizeof(buf));
      pos = 0, token_len = 0;
      switch (choose(3)) {
        case 0: gen_space(); gen_num(); gen_space(); ++token_len; break;
        case 1: gen_space(); gen('('); gen_space(); gen_rand_expr(0); gen_space(); gen(')'); gen_space(); token_len += 2; break; 
        default: gen_space(); gen_rand_expr(0); gen_space(); gen_rand_op(); gen_space(); gen_rand_expr(0); gen_space(); ++token_len; break;
      }
    } while(token_len > 32 || pos > 65535 || !chk());
  }
  else {
    switch(choose(3)){
      case 0: gen_num(); ++token_len; break;
      case 1: gen('('); gen_rand_expr(0); gen(')'); token_len += 2; break;
      default: gen_rand_expr(0); gen_rand_op(); gen_rand_expr(0); ++token_len; break;
    }
  }
  return;
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  buf[0] = '\0';
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr(1);
    assert(buf[0] != '\0');
    printf("%s\n", buf);
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
