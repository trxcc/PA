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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_NUM = 1, TK_EQ = 2, TK_REG = 3, TK_VAR = 4, TK_HEX = 5,
  TK_AND = 6, TK_OR = 7, TK_POINTER = 8, TK_NEQ = 9,
  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus
  {"\\*", '*'},         // multiply
  {"\\/", '/'},         // divide
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},
  {"\\$[\\$a-z0-9]+", TK_REG}, // register
  {"\\b[0-9]+\\b", TK_NUM},  // number
  {"\\b[A-Za-z]+\\b", TK_VAR},    // variable
  {"0[xX][0-9A-Fa-f]+", TK_HEX},   // hex_number
  {"\\&\\&", TK_AND},          // and
  {"\\|\\|", TK_OR},           // or

  {"\\(", '('},
  {"\\)", ')'},
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[320] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static inline void record_token(char *r, int len, int Nr_token, int token_type){
  memset(tokens[Nr_token].str, '\0', sizeof(tokens[Nr_token].str));
  strncpy(tokens[Nr_token].str, r, len);
  tokens[Nr_token].type = token_type;
  return;   // record the token into tokens[nr_token].str
}

extern word_t isa_reg_str2val(const char *s, bool *success);

static bool make_token(char *e) {
  int position = 0;
  int i;
  //printf("%s\n", e);
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        
        switch (rules[i].token_type) {
          case TK_NOTYPE: break;
          case TK_REG:
            record_token(&e[position-substr_len+1], substr_len-1, nr_token, rules[i].token_type);
            bool flag = true;
            word_t ans = isa_reg_str2val(tokens[nr_token].str, &flag);
            if (!flag) { printf("Invalid addr.\n"); assert(0); }
            memset(tokens[nr_token].str, '\0', sizeof(tokens[nr_token].str));
            sprintf(tokens[nr_token].str, "%u", ans);
            ++nr_token;
            printf("%d\n", tokens[--nr_token].type);
            printf("%s\n", tokens[nr_token++].str);
            break; 
          case TK_HEX:
            if (substr_len > 32) {
              printf("Too long token in position %d! Please check again!\n", position-substr_len);       
              return false;
          }
            char tmp_str[32]; memset(tmp_str, '\0', sizeof(tmp_str));
            strncpy(tmp_str, &e[position-substr_len+2], substr_len-2);
            int a;
            sscanf(tmp_str, "%x", &a);
            printf("%d\n", a);
            sprintf(tokens[nr_token].str, "%d", a);
            tokens[nr_token].type = TK_HEX;
            ++nr_token;
            printf("%d\n", tokens[--nr_token].type);
            printf("%s\n", tokens[nr_token++].str);
            break;
          case '+': case '-': case '*':
          case '/': case '(': case ')': 
          case TK_EQ: case TK_VAR: 
          case TK_AND: case TK_OR:
          case TK_NEQ:
          case TK_NUM:
            if (substr_len > 32){
              printf("Too long token in position %d! Please check again!\n", position-substr_len);
              return false;
            }
            record_token(&e[position - substr_len], substr_len, nr_token++, rules[i].token_type);
            printf("%d\n", tokens[--nr_token].type);
            printf("%s\n", tokens[nr_token++].str);
            break; 
          default: printf("Invalid input expression! Please check again!\n"); break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  return true;
}

static bool check_parentheses(int p, int q){
  if (tokens[p].type != '(' || tokens[q].type != ')') { return false; }
  int cnt = 1, i = p + 1;
  bool flag = true;
  for (; i < q; i++){
    if (tokens[i].type == '(') ++cnt;
    else if (tokens[i].type == ')') --cnt;
    //printf("cnt:%d\n", cnt);
    if (cnt == 0) { flag = false; }
  }
  if (cnt != 1) { printf("iiInvalid expression.\n"); assert(0); }
  return flag;
}

static int get_op_type(int p, int q){
  int ans = -1, cnt = 0; bool flag = true;
  for (int i = p; i <= q; i++) {
    if (tokens[i].type == TK_NUM || tokens[i].type == TK_HEX || tokens[i].type == TK_REG) { continue; }
    else if (tokens[i].type == ')') { 
      --cnt; 
      if (cnt == 0) { flag = true; }
    }
    else if (tokens[i].type == '(') { flag = false; ++cnt; }
    else if (flag == false) { continue; }
    else if (ans == -1) { /*printf("%d\n", cnt);*/ ans = i; }
    else if (tokens[ans].type == '+' || tokens[ans].type == '-') {
      if (tokens[i].type == '+' || tokens[i].type == '-') { ans = i; }
    }
    else if (tokens[ans].type == '*' || tokens[ans].type == '/') {
      if (tokens[i].type == TK_EQ || tokens[i].type == TK_NEQ || tokens[i].type == TK_AND ||tokens[i].type == TK_POINTER) {continue;}
      ans = i;
    }
    else if (tokens[ans].type == TK_EQ || tokens[ans].type == TK_NEQ || tokens[ans].type == TK_AND){
      if (tokens[i].type == TK_POINTER) { continue; }
      ans = i;
    }
    else if (tokens[ans].type == TK_POINTER) { ans = i; }
  }
  return ans;
}

word_t eval(int p, int q) {
  //printf("p:%d q:%d\n", p, q);
  if (p > q) { printf("opInvalid expression.\n"); assert(0); }
  else if (p == q) { int ans; sscanf(tokens[p].str, "%d", &ans); return ans; }
  else if (check_parentheses(p, q) == true) {
    return eval(p + 1, q - 1);
  }
  else {
    //op = the position of the main operation in the token expression.
    int op = get_op_type(p, q);
    //printf("op:%d, %s \n", op, tokens[op].str);//hhh
    word_t val1 = eval(p, op - 1), val2 = eval(op + 1, q);
    
    switch(tokens[op].type){
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': if (val2 == 0) { printf("Divide 0 error occured!\n"); return 0;}return val1 / val2;
      case TK_EQ: return val1 == val2;
      case TK_NEQ: return val1 != val2;
      case TK_AND: return val1 && val2;
      case TK_POINTER: 
      default: assert(0);
    }
  }
}

/*static void dereference() {
  for(int i = 0; i < nr_token; i++) {
    if (tokens[i].type == TK_POINTER) {
      vaddr_t addr = 0;
      memset(tokens[i].str, '\0', sizeof(tokens[i].str));
      sscanf(tokens[i+1].str, "%d", &addr);
      uint32_t data = vaddr_read(addr, 1);
      sprintf(tokens[i].str, "%u", data); 
      tokens[i].type = TK_NUM;
 
      for (int j = i+1; j < nr_token-1; j++){
        tokens[j].str = tokens[j+1].str, tokens[j].type = tokens[j+1].type;
      }
      tokens[nr_token-1].type = 0;
      memset(tokens[nr_token-1].str, '\0', siezof(tokens[nr_tokens-1].str));
      --nr_token;
  }
}
*/
word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  
  for(int i = 0; i < nr_token; i++){
    if (tokens[i].type == '*' && (i == 0 || (tokens[i - 1].type != TK_NUM && tokens[i - 1].type != TK_HEX)))
      { tokens[i].type = TK_POINTER;}
  }
  
  //dereference();

  *success = true;
  word_t ans = eval(0, nr_token - 1);
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  return ans;
  //return 0;
}
