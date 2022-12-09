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
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

const char *csrs[] = {"mepc", "mstatus", "mcause", "mtvec"};

void isa_reg_display() {
  //Log("hhh");
  for (int i = 0; i < 32; i++){
    printf("%s    ", reg_name(i, 8));
    if(strlen(reg_name(i, 8)) == 2) printf(" ");
    printf("0x%08X\n", cpu.gpr[i]);
  }
  printf("-------------------------------------------\n");
}

static int reg_index(const char *s){
  for (int i = 0; i < 32; i++) {
    bool flag = true;
    if (strlen(s) != strlen(regs[i])) {continue;} 
    for (int j = 0; j < strlen(s); j++){
      if (s[j] != regs[i][j]) {flag = false; break;}
    }
    if (flag) return i;
  }
  return -1; 
}

word_t isa_reg_str2val(const char *s, bool *success) {
  int i = reg_index(s);
  if (i == -1) { success = false; return 0; }
  *success = true;
  return cpu.gpr[i];
}

//word_t csr[4096];
static bool is_csr_inited = false;

static void csr_init() {
  if (is_csr_inited) return;
  for (int i = 0; i < 4096; i++) {
    csr[i] = 0;
  }
}

word_t get_csr_index(word_t index) {
  if (!is_csr_inited) csr_init();
  is_csr_inited = true;
  assert (index >= 0 && index <= 4096);
  return index;
}
